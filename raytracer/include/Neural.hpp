/*
    Header only library
    Purpose: Neural Network classes and utilities.

    Created by: Willis Medwell
    Date: 9/6/2023
*/
#pragma once

#include "Math.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iostream>
#include <numeric>
#include <random>
#include <ranges>
#include <vector>

struct NeuralTrainingSample {
    std::vector<float> in;
    std::vector<float> out;

    template <std::ranges::range In, std::ranges::range Out>
    constexpr NeuralTrainingSample(In input, Out output)
    {
        auto toFloat = [](auto& e) { return static_cast<float>(e); };
        std::ranges::copy(input | std::views::transform(toFloat), std::back_inserter(in));
        std::ranges::copy(output | std::views::transform(toFloat), std::back_inserter(out));
    }

    template <typename In, typename Out>
    constexpr NeuralTrainingSample(std::initializer_list<In> input, std::initializer_list<Out> output)
    {
        auto toFloat = [](auto& e) { return static_cast<float>(e); };
        std::ranges::copy(input | std::views::transform(toFloat), std::back_inserter(in));
        std::ranges::copy(output | std::views::transform(toFloat), std::back_inserter(out));
    }

    friend std::ostream& operator<<(std::ostream& os, const NeuralTrainingSample& sample)
    {
        std::cout << "std::vector<float>{ ";
        auto printWithComma = [&](const auto& e){ os << e << ","; };
        std::ranges::for_each(sample.in | std::views::take(sample.in.size() - 1), printWithComma);
        std::cout << sample.in.back() << "}, std::vector<float>{ ";
        std::ranges::for_each(sample.out | std::views::take(sample.out.size() - 1), printWithComma);
        std::cout << sample.out.back() << "}";
        return os;
    }
};

template <typename T>
concept NeuralTrainingRange = requires(T t) {
    requires std::ranges::range<T>;
    requires std::same_as<typename std::remove_cv<typename std::remove_reference<decltype(t[0])>::type>::type, NeuralTrainingSample>;
};

template <size_t num_inputs, class Func>
struct Neuron {
    constexpr static Func activation;
    std::array<float, num_inputs> weights;
    float bias;

    constexpr Neuron()
        : weights({})
        , bias(0)
    {
    }

    constexpr Neuron(const std::initializer_list<float>& w, float b)
    {
        assert(w.size() == num_inputs);

        std::ranges::copy(w, weights.begin());
        bias = b;
    }

    template <std::ranges::range Container>
        requires std::is_same<std::ranges::range_value_t<Container>, float>::value
    float propagateForward(const Container& inputs) const
    {
        assert(inputs.size() == weights.size());

        const float squared_sum = std::transform_reduce(
            weights.begin(), weights.end(),
            inputs.begin(),
            0.0f,
            std::plus {}, std::multiplies {});

        return activation(squared_sum + bias);
    }

    friend std::ostream& operator<<(std::ostream& os, const Neuron<num_inputs, Func>& n)
    {
        os << "{ {";
        for (auto w : n.weights | std::views::take(n.weights.size() - 1)) {
            os << w << "f, ";
        }
        os << n.weights.back() << "f}, " << n.bias;
        os << "f }";
        return os;
    }
};

template <size_t num_inputs, size_t num_neurons, class Func>
struct NeuralLayer {
    constexpr static size_t input_size = num_inputs;
    constexpr static size_t output_size = num_neurons;
    Func activation;
    std::array<Neuron<input_size, Func>, output_size> neurons;

    constexpr NeuralLayer(const std::initializer_list<Neuron<input_size, Func>>& n)
    {
        assert(n.size() == num_neurons);
        std::ranges::copy(n, neurons.begin());
    }
    constexpr NeuralLayer() = default;

    template <std::ranges::range Container>
        requires std::is_same<float, std::ranges::range_value_t<Container>>::value
    std::array<float, output_size> propagateForward(const Container& input) const
    {
        auto propagate = [&](const auto& n) {
            return n.propagateForward(input);
        };

        std::array<float, output_size> result;
        std::ranges::transform(neurons, result.begin(), propagate);
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const NeuralLayer<num_inputs, num_neurons, Func> layer)
    {
        std::ranges::for_each(layer.neurons | std::views::take(layer.neurons.size() - 1), [&](const auto& n) { os << n << ", "; });
        os << layer.neurons.back();
        return os;
    }
};

template <typename T>
struct IsNeuralLayer : std::false_type { };

template <size_t num_inputs, size_t num_neurons, class Func>
struct IsNeuralLayer<NeuralLayer<num_inputs, num_neurons, Func>> : std::true_type { };

template <typename... Layers>
    requires(IsNeuralLayer<Layers>::value && ...)
struct NeuralNetwork {
    /*
        Stored in a tuple as each layer can be of differing lengths.
        - don't want std::vectors everywhere fragmenting my memory.
        - gives compiler way more information, allowing for better code generation (SIMD etc).
    */
    using LayersTuple = std::tuple<Layers...>;

    LayersTuple layers;

    constexpr static size_t layers_size = std::tuple_size<LayersTuple>();
    constexpr static size_t output_size = std::tuple_element_t<std::tuple_size_v<LayersTuple> - 1, LayersTuple>::output_size;
    constexpr static size_t input_size = std::tuple_element_t<0, LayersTuple>::input_size;

    constexpr NeuralNetwork(Layers... args)
        : layers(std::make_tuple<Layers...>(std::forward<Layers>(args)...))
    {
    }

    constexpr NeuralNetwork() = default;

    constexpr NeuralNetwork(const NeuralNetwork<Layers...>& network)
        : layers(network.layers)
    {
        static_assert((IsNeuralLayer<Layers>::value && ...), "All types must be NeuralLayer");
    }

    constexpr NeuralNetwork<Layers...>& operator=(const NeuralNetwork<Layers...>& other)
    {
        layers = other.layers;
        return *this;        
    }

    using OutputContainer = std::array<float, output_size>;
    using InputContainer = std::array<float, input_size>;

public:
    template <std::ranges::range Container>
        requires std::is_same<std::ranges::range_value_t<Container>, float>::value
    OutputContainer propagateForward(const Container& inputs) const
    {
        assert(inputs.size() == input_size);
        auto propForward = [](auto& input, auto& layer) {
            return layer.propagateForward(input);
        };
        return accumulateTuple(layers, inputs, propForward);
    }

    consteval static size_t getNeuronCount()
    {
        // hack but we like that.
        LayersTuple tuple;
        return accumulateTuple(tuple, size_t(0), [](size_t s, const auto& layer) { return s + layer.neurons.size(); });
    }

    auto randomizeBetter()
    {
        constexpr float mean = 0.0f;
        constexpr float standard_deviation = Math::sqrt(2.0f / getNeuronCount());

        thread_local static std::random_device rd {};
        std::mt19937 gen { rd() };
        std::normal_distribution<> distribution { mean, standard_deviation };

        auto randomizeNeuron = [&](auto& neuron) {
            std::ranges::generate(neuron.weights, [&]() { return distribution(gen); });
            neuron.bias = 0.0f;
        };
        auto randomizeLayer = [&](auto& layer) { std::ranges::for_each(layer.neurons, randomizeNeuron); };
        forEachTuple(layers, randomizeLayer);
    }

    friend std::ostream& operator<<(std::ostream& os, NeuralNetwork<Layers...>& network)
    {
        os << "NeuralNetwork network = {\n";
        network.forEachTuple(network.layers, [&](const auto& layer) { os << "\tNeuralLayer<" << layer.input_size << ", " << layer.output_size << ", " << layer.activation << ">({" << layer << "}),\n"; });
        os << "};\n";
        return os;
    }

    float performance(const NeuralTrainingRange auto& training_data) const
    {
        auto getSampleCost = [&](const auto& sample) -> float {
            return cost(layers, sample.in, sample.out);
        };

        const float total_cost = std::transform_reduce(
            training_data.begin(), training_data.end(),
            0.0f,
            std::plus(),
            getSampleCost);

        return total_cost / (float)training_data.size();

        // If std::ranges::reduce or std::ranges::accumulate exisited this would be nicer.
    }

    void train(const NeuralTrainingRange auto& training_data, size_t epoch, float h, float rate)
    {
        for (size_t i = 0; i < epoch; i++) {
            for (const auto& sample : training_data) {
                const auto out = cost(layers, sample.in, sample.out);
                auto updated = layers;
                trainEachLayer(&updated, sample, out, h, rate);
                layers = updated;
            }
        }
    }

    template<size_t random_networks_count>
    void trainManyRandomNetworks(const NeuralTrainingRange auto& training_data, size_t epoch, float h, float rate)
    {
        std::vector<NeuralNetwork<Layers...>> networks;
        networks.resize(random_networks_count);
        std::ranges::for_each(networks, [](auto& network) { network.randomizeBetter(); });
        std::for_each(networks.begin(), networks.end(), [&](auto& network) { network.train(training_data, epoch, h, rate); });
        auto bestPerformance = [&](const auto& lhs, const auto& rhs) {
            return lhs.performance(training_data) < rhs.performance(training_data);
        };
        *this = *std::ranges::min_element(networks, bestPerformance);
    }

private: // Methods to iterate through the layers tuple. Tried to match std::algorithm for some consistency.
    template <size_t index = 0, typename Tuple, typename T, class Pred>
    constexpr static auto accumulateTuple(const Tuple& tuple, const T& init, Pred predicate)
    {
        if constexpr (index == std::tuple_size<Tuple>()) {
            return init;
        } else {
            auto& layer = std::get<index>(tuple);
            return accumulateTuple<index + 1>(tuple, predicate(init, layer), predicate);
        }
    }

    // mutable version
    template <size_t index = 0, typename Tuple, typename T, class Pred>
    constexpr static auto accumulateTuple(Tuple& tuple, T& init, Pred predicate)
    {
        if constexpr (index == std::tuple_size<Tuple>()) {
            return init;
        } else {
            auto& layer = std::get<index>(tuple);
            return accumulateTuple<index + 1>(tuple, predicate(init, layer), predicate);
        }
    }

    template <size_t index = 0, typename Tuple, class Pred>
    constexpr static void forEachTuple(Tuple& tuple, Pred predicate)
    {
        if constexpr (index == std::tuple_size<Tuple>()) {
            return;
        } else {
            auto& layer = std::get<index>(tuple);
            predicate(layer);
            return forEachTuple<index + 1>(tuple, predicate);
        }
    }

    // mutable version
    template <size_t index = 0, typename Tuple, class Pred>
    constexpr static void forEachTuple(const Tuple& tuple, Pred predicate)
    {
        if constexpr (index == std::tuple_size<Tuple>()) {
            return;
        } else {
            auto& layer = std::get<index>(tuple);
            predicate(layer);
            return forEachTuple<index + 1>(tuple, predicate);
        }
    }

    template <size_t index = 0>
    constexpr void trainEachLayer(LayersTuple* updated_layers, const NeuralTrainingSample& sample, const float& c, float h = 1e-2f, float rate = 1e-2f) const
    {
        if constexpr (index == layers_size) {
            return;
        } else {
            // train each neuron
            auto& updated_layer = std::get<index>(*updated_layers);
            LayersTuple temp = layers;

            for (size_t n = 0; n < updated_layer.neurons.size(); n++) {
                for (size_t w = 0; w < updated_layer.neurons[n].weights.size(); w++) {
                    (std::get<index>(temp)).neurons[n].weights[w] += h;
                    float dw = (cost(temp, sample.in, sample.out) - c) / h;
                    updated_layer.neurons[n].weights[w] -= rate * dw;
                    (std::get<index>(temp)).neurons[n].weights[w] = std::get<index>(layers).neurons[n].weights[w];
                }
                std::get<index>(temp).neurons[n].bias += h;
                float db = (cost(temp, sample.in, sample.out) - c) / h;
                updated_layer.neurons[n].bias -= rate * db;
                std::get<index>(temp).neurons[n].bias = std::get<index>(layers).neurons[n].bias;
            }
            // train next layer
            return trainEachLayer<index + 1>(updated_layers, sample, c);
        }
    }

    template <std::ranges::range In, std::ranges::range Out>
        requires std::is_same<std::ranges::range_value_t<In>, float>::value
        && std::is_same<std::ranges::range_value_t<Out>, float>::value
    static float cost(const LayersTuple& layers, const In& inputs, const Out& expected)
    {
        auto propForward = [](auto& input, auto& layer) {
            return layer.propagateForward(input);
        };
        OutputContainer actual = accumulateTuple(layers, inputs, propForward);

        return std::transform_reduce(
            actual.begin(), actual.end(),
            expected.begin(),
            0.0f,
            std::plus {},
            [](float lhs, float rhs) { return (lhs - rhs) * (lhs - rhs); });
    }
};
