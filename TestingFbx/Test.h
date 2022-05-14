#pragma once
#include <cmath>
#include <cstdlib>
#include <random>

class TestClass {
public:
	std::default_random_engine gen;
	std::normal_distribution<float> distribution = std::normal_distribution<float>(0.0, 1.0f);

	TestClass() {

	}

private:


};