#include <SFML/Graphics.hpp>
#include <chrono> // to get the time
#include <iostream>
#include <random> // to get random values

struct Timer {
  std::chrono::time_point<std::chrono::steady_clock> start, end;
  std::chrono::duration<float> duration;

  Timer() { start = std::chrono::high_resolution_clock::now(); }

  ~Timer() {
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    float ms = duration.count() * 1000.0f;
    std::cout << "Timer took " << ms << "ms" << std::endl;
  }
};

class Complex {
public:
  Complex(double r = 0.0, double i = 0.0) : _r(r), _i(i) {}

  double r() const { return _r; }
  double i() const { return _i; }

  Complex operator*(const Complex &other) {
    return Complex(_r * other._r - _i * other._i,
                   _r * other._i + _i * other._r);
  }

  Complex operator+(const Complex &other) {
    return Complex(_r + other._r, _i + other._i);
  }

  double sqMagnitude() const { return _r * _r + _i * _i; }

private:
  double _r, _i;
};

int rowFromReal(double real, double min, double max, int screenWidth) {
  return static_cast<int>((real - min) * screenWidth / (max - min));
}

int colFromImag(double imag, double min, double max, int screenWidth) {
  return static_cast<int>((imag - min) * screenWidth / (max - min));
}

std::vector<Complex> buddhabrotPoint(Complex &c, int maxIter) {
  std::vector<Complex> toReturn;
  toReturn.reserve(maxIter);
  Complex z;
  int counter = 0;
  while (counter < maxIter && z.sqMagnitude() <= 2.0) {
    z = z * z + c;

    toReturn.push_back(z);

    ++counter;
  }

  if (counter == maxIter)
    return std::vector<Complex>();
  else
    return toReturn;
}

int main() {
  const int winSize = 1000;
  sf::RenderWindow window(sf::VideoMode(winSize, winSize),
                          "Buddhabrot Visualization");

  sf::Image image;
  image.create(winSize, winSize, sf::Color(0, 0, 0));
  sf::Texture texture;
  sf::Sprite sprite;

  const Complex minimum(-2.0, -1.5);
  const Complex maximum(1.0, 1.5);
  const int maxPoints = winSize * winSize * 30;
  const int maxIter = 100;

  int **nPass = new int *[winSize];
  for (int i = 0; i < winSize; i++) {
    nPass[i] = new int[winSize];
    for (int j = 0; j < winSize; j++)
      nPass[i][j] = 0;
  }

  // Seeding the random generator
  std::mt19937 rng;
  std::uniform_real_distribution<double> realDistribution(minimum.r(),
                                                          maximum.r());
  std::uniform_real_distribution<double> imagDistribution(minimum.i(),
                                                          maximum.i());
  rng.seed(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  bool changedSomething = true;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (changedSomething) {
      Timer timer;
      for (int k = 0; k < maxPoints; ++k) {
        Complex c(realDistribution(rng), imagDistribution(rng));

        // Checks if point is in main cardioid
        double p =
            sqrt(((c.r() - 1 / 4.0) * (c.r() - 1 / 4.0)) + (c.i() * c.i()));
        double x = p - (2 * p * p) + 1 / 4.0;
        if (c.r() <= x)
          continue;

        // Checks if point is in the second cardioid
        x = (c.r() + 1) * (c.r() + 1) + c.i() * c.i();
        if (x <= 1 / 16.0)
          continue;

        std::vector<Complex> pointPassed = buddhabrotPoint(c, maxIter);

        for (Complex n : pointPassed) {
          if (n.i() <= maximum.i() && n.i() >= minimum.i() &&
              n.r() <= maximum.r() && n.r() >= minimum.r()) {
            int row = rowFromReal(n.r(), minimum.r(), maximum.r(), winSize);
            int col = colFromImag(n.i(), minimum.i(), maximum.i(), winSize);
            ++nPass[row][col];
          }
        }

        if (k == maxPoints / 2)
          std::cout << "Half of the points calculated!" << std::endl;
      }

      int maxPass = 0;
      for (int y = 0; y < winSize; ++y) {
        for (int x = 0; x < winSize; ++x) {
          if (nPass[y][x] > maxPass)
            maxPass = nPass[y][x];
        }
      }

      for (int y = 0; y < winSize; y++) {
        for (int x = 0; x < winSize; x++) {
          int colorValue = (int)(255 * nPass[y][x] / maxPass);
          image.setPixel(x, y, sf::Color(colorValue, colorValue, colorValue));
        }
      }

      std::cout << "Finished Plotting" << std::endl;
    }

    texture.loadFromImage(image);
    sprite.setTexture(texture);

    window.clear();
    window.draw(sprite);
    window.display();
    changedSomething = false;
  }

  for (int i = 0; i < winSize; ++i) {
    delete[] nPass[i];
    nPass[i] = nullptr;
  }

  delete[] nPass;

  return 0;
}
