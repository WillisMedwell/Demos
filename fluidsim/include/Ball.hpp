#include "LinearAlgebra.hpp"

struct Ball {
    LinearAlgebra::Vec<2, float> velocity;
    LinearAlgebra::Vec<2, float> acceleration;
    LinearAlgebra::Pos<2, float> position;
    float radius;
    float mass;

    friend constexpr bool doesIntersect(const Ball& lhs, const Ball& rhs)
    {
        return LinearAlgebra::distance(lhs.position, rhs.position) < lhs.radius + rhs.radius;
    } 
    friend constexpr void applyCollison(const Ball& lhs, const Ball& rhs)
    {

    }
    constexpr void applyForces(float dt)
    {
        namespace LA = LinearAlgebra;
        velocity = velocity + (LA::Vec<2,float>(acceleration) * dt); 
        position = LA::pos<2,float>(LA::Vec<2,float>(position) + (velocity * dt));         
    }
};

