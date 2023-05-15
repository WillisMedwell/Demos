#define NDEBUG
#include <array>
#include <variant>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <string>
#include <cassert>
#include <vector>
#include <cmath>
//#include <iostream>
//#include <iomanip>
//#include <chrono>

template<typename T>
requires std::is_integral<T>::value || std::is_floating_point<T>::value
struct Pos2
{
    T x, y;
    constexpr Pos2()
    :   x(0), y(0)
    {}
    constexpr Pos2(T x_, T y_)
    :   x(x_), y(y_)
    {}
    friend constexpr inline auto operator<(const Pos2<T>& lhs, const Pos2<T>& rhs) -> bool
    { 
        if(lhs.x < rhs.x || (lhs.x == rhs.y && lhs.y < rhs.y)) 
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
    friend constexpr inline auto operator==(const Pos2<T>& lhs, const Pos2<T>& rhs) -> bool
    { 
        return lhs.x == rhs.x && lhs.y == rhs.y; 
    }
    inline auto GetDebugString() const
    {
        using namespace std;
        if(x == std::numeric_limits<T>::max())
        {
            return string("null");
        }
        return string("(" + to_string(x) + ", " + to_string(y) + ")");
    }
    template <typename U>
    operator Pos2<U>() const {
        return Pos2<U>{static_cast<U>(x), static_cast<U>(y)};
    }
};


// wrapper for a vector that acts as if it was 2 dimensional. Conveniant and contigious.
template<typename T>
class Vector2D
{
private:
    std::vector<T> data;
    Pos2<size_t> dimensions;
public:
    inline auto size() const noexcept -> Pos2<size_t>
    {
        return dimensions;
    }
    inline auto resize(const size_t x, const size_t y) noexcept -> void
    {
        dimensions.x = x;
        dimensions.y = y;
        data.resize(x*y);
    }
    
    inline Vector2D(const size_t x, const size_t y)
    {
        resize(x,y);
    }

    inline auto at(const size_t x, const size_t y) noexcept -> T&
    {
        assert(x < dimensions.x && y < dimensions.y && "index out of grid bounds");
        return data.at(x*dimensions.y + y);
    }
    inline auto at(const Pos2<size_t>& index) noexcept
    {
        assert(index.x < dimensions.x && index.y < dimensions.y && "index out of grid bounds");
        return data.at(index.x*dimensions.y + index.y);
    }
    inline auto operator[](const Pos2<size_t>& index) noexcept -> T&
    {
        assert(x < dimensions.x && y < dimensions.y && "index out of grid bounds");
        return data[index.x*dimensions.y + index.y];
    }
    inline auto indexOf(const T* element) noexcept -> Pos2<size_t>
    {
        assert(element >= std::to_address(data.begin()) && element < std::to_address(data.end()) && "element out of grid bounds");
        const int64_t index = element - std::to_address(data.begin()); 
        assert(index >= 0 && "index out of grid bounds");
        const uint64_t x = static_cast<uint64_t>(index) / dimensions.y;
        const uint64_t y = static_cast<uint64_t>(index) % dimensions.y;
        return Pos2<size_t>(x,y);
    }
    inline auto begin() noexcept
    {
        return data.begin();
    }
    inline auto end() noexcept
    {
        return data.end();
    }
};

template<typename T>
concept Is2Dimensional = requires 
{ 
    std::ranges::random_access_range<T> && 
    std::ranges::random_access_range<std::ranges::range_value_t<T>> && 
    std::is_integral<std::ranges::range_value_t<std::ranges::range_value_t<T>>>::value; 
};

template<typename T> inline auto getAdjacentPositions(const Pos2<T>& pos)
{
    return std::to_array({
        Pos2<T>(pos.x + 1, pos.y),
        Pos2<T>(pos.x - 1, pos.y),
        Pos2<T>(pos.x,     pos.y + 1),
        Pos2<T>(pos.x,     pos.y - 1)
    }); 
}
template<typename T> inline auto getDiagonalPositions(const Pos2<T>& pos)
{
    return std::to_array({
        Pos2<T>(pos.x + 1, pos.y + 1),
        Pos2<T>(pos.x - 1, pos.y + 1),
        Pos2<T>(pos.x + 1, pos.y - 1),
        Pos2<T>(pos.x - 1, pos.y - 1)
    }); 
}
template<typename T> inline auto getSurroundingPositions(const Pos2<T>& pos)
{
    return std::to_array({
        Pos2<T>(pos.x - 1, pos.y - 1),
        Pos2<T>(pos.x - 1, pos.y),
        Pos2<T>(pos.x - 1, pos.y + 1),

        Pos2<T>(pos.x,     pos.y + 1),
        Pos2<T>(pos.x,     pos.y - 1),

        Pos2<T>(pos.x + 1, pos.y - 1),
        Pos2<T>(pos.x + 1, pos.y),
        Pos2<T>(pos.x + 1, pos.y + 1)
    }); 
}

class Astar
{
public:
    struct AstarCost
    {
        using Underlying = float;
        float g, f;
        auto getDebugString() -> std::string
        {
            std::string output;
            output += "(" + std::to_string(g) + ", " + std::to_string(f) + ") ";
            return output;
        }
        AstarCost(float g_, float f_)
        :   g(g_), f(f_)
        {}
    };
    struct OpenNode
    {
        AstarCost cost;
        OpenNode* parent;
        OpenNode* child;
        auto getDebugString() -> std::string
        {
            return cost.getDebugString();
        } 
        OpenNode(const AstarCost& astarCost)
        :   cost(astarCost), parent(nullptr), child(nullptr)
        {}
    };
    
    using Unassigned = int;
    using ClosedElement = AstarCost;
    using Variant = std::variant<Unassigned, OpenNode, ClosedElement>;

    using GridIndex = Pos2<uint64_t>;

    std::optional<OpenNode*> beginOpenList;

    Vector2D<Variant> grid;
    Vector2D<GridIndex> prev;

    inline auto doesOpenListContain(OpenNode* searchNode)
    {
        if(!beginOpenList.has_value())
        {
            return false;
        }

        OpenNode* iter = beginOpenList.value(); 
        while(iter != nullptr)
        {
            if(iter == searchNode)
            {
                return true;
            }
            iter = iter->child;
        }
        return false;
    }
    inline auto removeOpenNodeFromList(OpenNode& node)
    {
        assert(beginOpenList.has_value());

        // beginning of list setup
        if(beginOpenList.value() == &node)
        {
            // has no child AKA only element in list
            if(node.child == nullptr)
            {
                beginOpenList = std::nullopt;
            }
            else
            {
                beginOpenList = node.child;
            }
        }

        if(node.child != nullptr)
        {
            node.child->parent = node.parent;
        }

        if(node.parent != nullptr)
        {
            node.parent->child = node.child;
        }

        node.child = nullptr;
        node.parent = nullptr;

        assert(!this->doesOpenListContain(&node));
    }
    inline auto setInitialConditions()
    {
        std::fill(grid.begin(), grid.end(), Unassigned());
        beginOpenList = std::nullopt;
    }
    // all related to the open list
    inline auto insertOpenNode(const GridIndex& pos, const AstarCost newCost)
    {
        // ensure that it has not been assigned as a closed element. And is within boundaries.
        // assert(!std::holds_alternative<ClosedElement>(grid.at(pos)));
        Variant& entry = grid[pos];

        bool openListEmpty = !beginOpenList.has_value();
        if(openListEmpty)
        {
            // put to front of list.
            entry = OpenNode(newCost);
            beginOpenList = &std::get<OpenNode>(entry);
            return;
        }

        bool isAssigned = std::holds_alternative<OpenNode>(entry);
        if(isAssigned)
        {
            OpenNode& node = std::get<OpenNode>(entry);
            if(newCost.f < node.cost.f)
            {
                // remove from list.
                this->removeOpenNodeFromList(node);
            }
            else 
            {
                // no point inserting
                return;
            }
        }
        else
        {
            // its a Unassigned node, meaning a simple assignment is good enough.
            entry = OpenNode(newCost);   
        }

        OpenNode& node = std::get<OpenNode>(entry);
        node.cost = newCost;

        // insert into beginning of list
        if(newCost.f <= beginOpenList.value()->cost.f)
        {
            beginOpenList.value()->parent = &node;
            node.child = beginOpenList.value();
            beginOpenList = &node;

            assert(this->doesOpenListContain(&node));
            return;
        }

#if 1
        auto insertThroughChildrenFrom = [&](OpenNode& node, OpenNode* iter)
        {
            OpenNode* prev_iter = nullptr;
            while(iter != nullptr)
            {
                if(node.cost.f <= iter->cost.f)
                {
                    node.parent = iter->parent;
                    if(iter->parent != nullptr)
                    {
                        iter->parent->child = &node;
                    }
                    node.child = iter;
                    iter->parent = &node;
                    assert(doesOpenListContain(&node));
                    return;
                }
                prev_iter = iter;
                iter = iter->child;
            }
            prev_iter->child = &node;
            node.parent = prev_iter;
        };
        auto insertThroughParentsFrom = [&](OpenNode& node, OpenNode* iter)
        {
            while(iter != beginOpenList.value())
            {
                if(node.cost.f <= iter->cost.f)
                {
                    //InsertNodeAsParent(new_node, iter_node);
                    node.parent = iter->parent;
                    iter->parent->child = &node;
                    iter->parent = &node;
                    node.child = iter;
                    return;
                }
                iter = iter->parent;
            }
        };
        const auto surrounding = getSurroundingPositions(pos);
        auto nearby_pos_iter = std::ranges::find_if(surrounding, 
            [&](const Pos2<size_t>& nearby)
            {
                // check to see if its a valid node.
                return nearby.x < grid.size().x && nearby.y < grid.size().y && std::holds_alternative<OpenNode>(grid[nearby]);
            }
        ); 
        
        if(nearby_pos_iter == surrounding.end())
        {
            insertThroughChildrenFrom(node, beginOpenList.value());
            assert(this->doesOpenListContain(&node));
            return;
        }
        
        OpenNode& nearby_node = std::get<OpenNode>(grid[*nearby_pos_iter]);

        bool search_parents = node.cost.f < nearby_node.cost.f;
        if(search_parents)
        {
            insertThroughParentsFrom(node, &nearby_node);
            
            assert(this->doesOpenListContain(&node));
            return;
        }
        else
        {
            insertThroughChildrenFrom(node, &nearby_node);
            
            assert(this->doesOpenListContain(&node));
            return;
        }
#else
        // insert down into list
        OpenNode* iter = beginOpenList.value();
        OpenNode* prev_iter = nullptr;
        while(iter != nullptr)
        {
            if(node.cost.f <= iter->cost.f)
            {
                node.parent = iter->parent;
                if(iter->parent != nullptr)
                {
                    iter->parent->child = &node;
                }
                node.child = iter;
                iter->parent = &node;

                assert(this->doesOpenListContain(&node));
                return;
            }
            prev_iter = iter;
            iter = iter->child;
        }
        prev_iter->child = &node;
        node.parent = prev_iter;
#endif
        assert(this->doesOpenListContain(&node));
    }
    inline auto getOpenListDebugString() const -> std::string
    {
        if(!beginOpenList.has_value())
        {
            return "OPEN LIST IS EMPTY";
        }

        std::string output =""; 

        OpenNode* iter = beginOpenList.value();
        while(iter != nullptr)
        {
            output += iter->getDebugString() + '\n';   
            // advance node
            iter = iter->child;
        }
        return output;
    }
    inline auto hasOpenList() const
    {
        return beginOpenList.has_value();
    }
    
    inline auto getFrontCost()
    {
        assert(beginOpenList.has_value() && hasOpenList());
        return beginOpenList.value()->cost;
    }
    inline auto getFrontIndex()
    {
        // ensure defined behaviour for assurance.
        assert(beginOpenList.has_value() && hasOpenList());
        assert(grid.indexOf(reinterpret_cast<Variant*>(beginOpenList.value())).x < grid.size().x);
        assert(grid.indexOf(reinterpret_cast<Variant*>(beginOpenList.value())).y < grid.size().y);
        assert(std::get<OpenNode>(grid.at(grid.indexOf(reinterpret_cast<Variant*>(beginOpenList.value())))).getDebugString() == beginOpenList.value()->getDebugString());

        return grid.indexOf(reinterpret_cast<Variant*>(beginOpenList.value()));
    }
    inline auto popFront()
    {
        assert(hasOpenList());
        this->removeOpenNodeFromList(*beginOpenList.value());
    }

    inline auto makeSizingRequirements(const Pos2<size_t>& size)
    {
        if(grid.size().x < size.x || grid.size().y < size.y)
        {
            grid.resize(size.x, size.y);
            prev.resize(size.x, size.y);
        }
    }

    inline auto calculateDistance(const Pos2<size_t>& initial, const Pos2<size_t>& final)
    {
        return static_cast<AstarCost::Underlying>(std::sqrt((final.x - initial.x)*(final.x - initial.x) + (final.y - initial.y)*(final.y - initial.y)));
    }

    template<Is2Dimensional CollisionGrid>
    [[nodiscard]] inline auto findPath(const CollisionGrid& collisionGrid, const Pos2<size_t>& start, const Pos2<size_t>& end)
    {
        const auto collisonDimensions = Pos2(collisionGrid.size(), collisionGrid.front().size()); 
        makeSizingRequirements(collisonDimensions);
        setInitialConditions();

        insertOpenNode(start, AstarCost(0, calculateDistance(start, end)));

        while(hasOpenList() && !std::holds_alternative<ClosedElement>(grid[end]))
        {
            const auto front_cost = getFrontCost();
            const auto front_pos  = getFrontIndex();
            popFront();

            grid[front_pos] = ClosedElement(front_cost);
            
            auto isInsideBoundary = [&](const Pos2<size_t>& pos)
            {
                return pos.x < collisonDimensions.x && pos.y < collisonDimensions.y; 
            };
            auto hasNoCollision = [&](const Pos2<size_t>& pos)
            {
                return collisionGrid[pos.x][pos.y] == 0; 
            };
            auto insert = [&](const AstarCost::Underlying new_g, const Pos2<size_t>& pos)
            {
                const bool is_closed = std::holds_alternative<ClosedElement>(grid[pos]);
                const bool is_open = std::holds_alternative<OpenNode>(grid[pos]);
                
                if(is_closed) 
                {
                    const bool swap_back_open = new_g < std::get<ClosedElement>(grid[pos]).g;
                    if(swap_back_open)
                    {
                        const auto new_value = AstarCost(new_g, new_g + calculateDistance(pos, end)); 
                        insertOpenNode(pos, new_value);
                        prev[pos] = front_pos;
                    }
                }
                else if(is_open && std::get<OpenNode>(grid[pos]).cost.g < new_g)
                {
                    return;
                }
                else
                {
                    const auto new_value = AstarCost(new_g, new_g + calculateDistance(pos, end));

                    insertOpenNode(pos, new_value);
                    prev[pos] = front_pos;
                }
            };
            /// Calculate cost of each surrounding positions.
            // N, E, S, W
            for(const auto& pos : getAdjacentPositions(front_pos) | std::views::filter(isInsideBoundary) | std::views::filter(hasNoCollision) )
            {
                insert(front_cost.g + 1.0f, pos);
            }
            // NE, SE, SW, NW
            for(const auto& pos : getDiagonalPositions(front_pos) | std::views::filter(isInsideBoundary) | std::views::filter(hasNoCollision) )
            {
                insert(front_cost.g + 1.41421f, pos);
            }
        }

        auto path = std::vector<Pos2<size_t>>();
        path.reserve(50);

        // work back from end
        path.emplace_back(end);
        while(path.back() != start)
        {
            const auto& back = path.back();
            path.emplace_back(prev.at(back));
        }
        std::ranges::reverse(path);

        // for(uint64_t y = 0; y < collisonDimensions.y; y++)
        // {
        //     for(uint64_t x = 0; x < collisonDimensions.x; x++)
        //     {
        //         auto iter = std::ranges::find(path, Pos2<uint64_t>(x,y));
        //         if(iter != path.end())
        //         {
        //             //std::cout << std::setw(4) << std::distance(path.begin(), iter);
        //             std::cout << std::setw(4) << "x";
        //             //std::cout << std::setw(8) << prev_pos[x][y].GetDebugString();
        //         }
        //         else if(collisionGrid[x][y])
        //         {
        //             std::cout << std::setw(4) << "|";   
        //         }
        //         else
        //         {
        //             // if(to_be_evaluated.Contains(Pos2(x,y)))
        //             // {
        //             //     std::cout <<std::setw(4) << static_cast<uint64_t>(to_be_evaluated.GetKeyValue(Pos2(x,y)).GetF());
        //             // }
        //             std::cout << std::setw(4) << "-";
        //         }
        //     }
        //     std::cout <<'\n';
        // }

        return path;
    }

    Astar()
    :   beginOpenList(std::nullopt), grid(0,0), prev(0,0)
    {}
};


template<size_t X, size_t Y> constexpr auto getTestValues()
{
    using CollisionGrid = std::array<std::array<bool, Y>, X>;
    CollisionGrid grid{};

    for(auto& element : grid[grid.size()/5*1])
    {
        element = true;
    }
    for(auto& element : grid[grid.size()/5*2])
    {
        element = true;
    }
    for(auto& element : grid[grid.size()/5*3])
    {
        element = true;
    }
    for(auto& element : grid[grid.size()/5*4])
    {
        element = true;
    }
    grid[grid.size()/5*1].back() = false;
    grid[grid.size()/5*2].front() = false;
    grid[grid.size()/5*3].back() = false;
    grid[grid.size()/5*4].front() = false;

    Pos2<uint64_t> start(0,0);
    Pos2<uint64_t> end(X-1, Y-1);

    return std::tuple(grid, start, end);
}

struct Rgba{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

static Pos2<size_t> start_pos{0,0};
static Pos2<size_t> end_pos{0,0};
static std::vector<std::vector<bool>> collision_grid;
static std::vector<Pos2<size_t>> path;
static Astar astar;
static Rgba pixels[300*600];



extern "C"
{
    void* getPixelDataOffset();
    void setCanvasSize(int x, int y);
    void setSize(int x, int y);
    void setWall(int x, int y, bool value);
    void setStart(int x, int y);
    void setEnd(int x, int y);
    void findPath();
}

void* getPixelDataOffset()
{
    return reinterpret_cast<void*>(pixels);
}


void setCanvasSize(int x, int y)
{

}

void setSize(int x, int y)
{
    collision_grid.resize(static_cast<size_t>(x));
    for(auto& v : collision_grid)
    {
        v.resize(static_cast<size_t>(y));
    }
}
void setWall(int x, int y, bool value)
{
    collision_grid.at(static_cast<size_t>(x)).at(static_cast<size_t>(y)) = value;
}
void setStart(int x, int y)
{
    start_pos = {static_cast<size_t>(x), static_cast<size_t>(y)};
}
void setEnd(int x, int y)
{
    end_pos = {static_cast<size_t>(x), static_cast<size_t>(y)};
}
void findPath()
{
    path = astar.findPath(collision_grid, start_pos, end_pos);
}

void paintPixels()
{

}

int main()
{

}
/*

int main()
{
    auto [grid, start, end] = getTestValues<64,64>();

    Astar astar;

    long long duration=0;
    size_t count = 0;
    size_t cycle_count = std::numeric_limits<size_t>::max();

    #ifdef NDEBUG
    for(int i = 0; i < 1000; i++) 
    #else
    for(int i = 0; i < 10; i++) 
    #endif
    {
        auto starttime = std::chrono::high_resolution_clock::now();
        const size_t start_cycle = __rdtsc();
        try 
        {
            const auto t1 = astar.findPath(grid, start, end);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what();
        }
        auto total = __rdtsc() - start_cycle;
        cycle_count = (total < cycle_count) ? total : cycle_count;
        duration += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - starttime).count();
        count++;
    }
    std::cout << "total " << std::setprecision(10) <<static_cast<double>(duration)/1'000'000/static_cast<double>(count) << "ms " << static_cast<double>(duration)/1'000/static_cast<double>(count) << "us " << cycle_count << "cylces/pathfind (min)\n";

    return 0;
}
*/
