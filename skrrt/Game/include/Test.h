#pragma once
#include <cmath>
#include <cstdlib>
#include <random>
#include <GL/glew.h>

class TestClass {
public:


	TestClass() {

	}

	TestClass(int i) {

	}

	float randomFloat() {

		distribution(gen);
		distribution(gen);

		return distribution(gen);
	}

private:
	std::default_random_engine gen;
	std::normal_distribution<float> distribution = std::normal_distribution<float>(0.0, 1.0f);


};