#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <random>
#include <utility>
#include <algorithm>
#include<vector>
#include "ga/operators/Crossover.h"

namespace ga {

    template<class Gene>
    class OrderCrossover : public Crossover<Gene> {
    public:
        OrderCrossover() = default;

        std::pair<Chromosome<Gene>, Chromosome<Gene>> cross(
            const Chromosome<Gene>& p1,
            const Chromosome<Gene>& p2,
            std::mt19937& rng) const override
        {
            if (p1.size() != p2.size())
                throw std::invalid_argument("Parents must have equal sizes");

            size_t n = p1.size();

            if (n < 2)
                return { p1, p2 };

            std::uniform_int_distribution<size_t> dist(0, n - 1);

            size_t cut1 = dist(rng);
            size_t cut2 = dist(rng);
            while (cut1 == cut2) cut2 = dist(rng);
            if (cut1 > cut2)
                std::swap(cut1, cut2);

            Chromosome<Gene> ch1(n);
            Chromosome<Gene> ch2(n);

            for (size_t i = cut1; i <= cut2; i++)
            {
                ch1[i] = p1[i];
                ch2[i] = p2[i];
            }

           
            size_t readIndex = (cut2 + 1) % n;
            size_t writeIndex = (cut2 + 1) % n;
            size_t remaining = n - (cut2 - cut1 + 1);

            while (remaining > 0)
            {
                Gene current = p2[readIndex];
                bool found = false;

                for (size_t i = cut1; i <= cut2; i++)
                {
                    if (ch1[i] == current)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {

                    ch1[writeIndex] = current;
                    writeIndex = (writeIndex + 1) % n;
                    remaining--;
                }

                readIndex = (readIndex + 1) % n;
            }

           
            readIndex = (cut2 + 1) % n;
            writeIndex = (cut2 + 1) % n;
            remaining = n - (cut2 - cut1 + 1);

            while (remaining > 0)
            {
                Gene current = p1[readIndex];
                bool found = false;

                for (size_t i = cut1; i <= cut2; i++)
                {
                    if (ch2[i] == current)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    if (writeIndex == cut1)
                        writeIndex = (cut2 + 1) % n;

                    ch2[writeIndex] = current;
                    writeIndex = (writeIndex + 1) % n;
                    remaining--;
                }

                readIndex = (readIndex + 1) % n;
            }

            return { ch1, ch2 };
        }

        std::string name() const override {
            return "Order (OX)";
        }
    };

}


