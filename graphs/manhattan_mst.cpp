#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <set>
#include <chrono>

constexpr double EPS = 1E-6;

struct Point2D {
  double x, y;
};

std::istream& operator>>(std::istream& in, Point2D& point) {
  return in >> point.x >> point.y;
}

std::ostream& operator<<(std::ostream& out, const Point2D& point) {
  return out << "(" << point.x << " " << point.y << ")";
}

double ManhattanDistance(const Point2D& point1, const Point2D& point2) {
  return fabs(point1.x - point2.x) + fabs(point1.y - point2.y);
}

constexpr int kMaxSize = 5;

std::vector<Point2D> all_points;
std::vector<int> all_neighbours;

template <typename Iterator>
void Merge(Iterator begin1, Iterator end1, Iterator begin2, Iterator end2,
           Iterator out) {
  auto it1 = begin1;
  auto it2 = begin2;
  while (it1 != end1 && it2 != end2) {
    if (all_points[*it1].y < all_points[*it2].y) {
      *(out++) = *(it1++);
    } else {
      *(out++) = *(it2++);
    }
  }
  while (it1 != end1) {
    *(out++) = *(it1++);
  }
  while (it2 != end2) {
    *(out++) = *(it2++);
  }
}

void UpdateNeighbour(int id, int new_neighbour) {
  if (all_points[id].x > all_points[new_neighbour].x
      || all_points[id].y > all_points[new_neighbour].y) return;
  if (new_neighbour >= 0) {
    if (all_neighbours[id] >= 0) {
      double cur_dist = ManhattanDistance(all_points[id], all_points[all_neighbours[id]]);
      double new_dist = ManhattanDistance(all_points[id], all_points[new_neighbour]);
      if (new_dist < cur_dist) {
        all_neighbours[id] = new_neighbour;
      }
    } else {
      all_neighbours[id] = new_neighbour;
    }
  }
}

void BruteNENeighbours(const std::vector<Point2D>& points) {
  for (int i = 0; i < points.size(); ++i) {
    double min_dist = std::numeric_limits<double>::max();
    int closest_id = -1;
    for (int j = 0; j < points.size(); ++j) {
      if (i == j
          || points[j].x < points[i].x
          || points[j].y < points[i].y) continue;
      double dist = ManhattanDistance(points[i], points[j]);
      if (dist < min_dist) {
        min_dist = dist;
        closest_id = j;
      }
    }
    if (closest_id >= 0 && all_neighbours[i] >= 0) {
      double dist = ManhattanDistance(points[i], points[all_neighbours[i]]);
      if (min_dist < dist) {
        all_neighbours[i] = closest_id;
      }
    } else if (closest_id >= 0) {
      all_neighbours[i] = closest_id;
    }
  }
}

void FindNENeighbours(std::vector<int>& points, int l, int r) {
  if (r - l < kMaxSize) {
    for (int i = l; i < r; ++i) {
      double min_dist = std::numeric_limits<double>::max();
      int closest_id = -1;
      for (int j = l; j < r; ++j) {
        if (points[i] == points[j]
            || all_points[points[j]].x < all_points[points[i]].x
            || all_points[points[j]].y < all_points[points[i]].y) continue;
        double dist = ManhattanDistance(all_points[points[i]], all_points[points[j]]);
        if (dist < min_dist) {
          min_dist = dist;
          closest_id = points[j];
        }
      }
      UpdateNeighbour(points[i], closest_id);
    }
    std::sort(points.begin() + l, points.begin() + r, [](int p1, int p2) {
      return all_points[p1].y < all_points[p2].y;
    });
  } else {
    FindNENeighbours(points, l, l + (r - l) / 2);
    FindNENeighbours(points, l + (r - l) / 2, r);
    std::vector<int> left_points(points.begin() + l, points.begin() + l + (r - l) / 2);
    std::vector<int> right_points(points.begin() + l + (r - l) / 2, points.begin() + r);
    int left = left_points.size() - 1;
    int right = right_points.size() - 1;
    int min_id = right_points.size() - 1;
    double min_dist = -1;
    while (left >= 0 && all_points[right_points[min_id]].y < all_points[left_points[left]].y) --left;
    if (left >= 0) {
      min_dist = ManhattanDistance(all_points[left_points[left]],
        all_points[right_points[min_id]]);
    }
    while (right > 0 && left >= 0) {
      --right;
      while (left >= 0 && all_points[right_points[right]].y < all_points[left_points[left]].y) {
        UpdateNeighbour(left_points[left], right_points[min_id]);
        --left;
        if (left >= 0) {
          min_dist = ManhattanDistance(all_points[left_points[left]],
            all_points[right_points[min_id]]);
        }
      }
      if (left >= 0) {
        double cur_dist = ManhattanDistance(all_points[left_points[left]],
          all_points[right_points[right]]);
        if (cur_dist < min_dist) {
          min_id = right;
          min_dist = cur_dist;
        }
      }
    }
    while (left >= 0) {
      UpdateNeighbour(left_points[left], right_points[min_id]);
      --left;
    }

    Merge(left_points.begin(), left_points.end(),
          right_points.begin(), right_points.end(),
          points.begin() + l);
  }
}

void CheckNENeighbours() {
  all_neighbours.assign(all_points.size(), -1);
  BruteNENeighbours(all_points);
  auto tmp = all_neighbours;
  all_neighbours.assign(all_points.size(), -1);

  std::vector<int> points(all_points.size());
  for (int i = 0; i < points.size(); ++i) {
    points[i] = i;
  }
  std::sort(points.begin(), points.end(), [](int p1, int p2) {
    return all_points[p1].x < all_points[p2].x;
  });
  FindNENeighbours(points, 0, points.size());
  for (int i = 0; i < all_points.size(); ++i) {
    if (all_neighbours[i] < 0) {
      assert(tmp[i] < 0);
      continue;
    }
    if (tmp[i] < 0) {
      assert(all_neighbours[i] < 0);
      continue;
    }
    double d1 = ManhattanDistance(all_points[i], all_points[all_neighbours[i]]);
    double d2 = ManhattanDistance(all_points[i], all_points[tmp[i]]);
    assert(fabs(d1 - d2) < EPS);
  }
  std::cout << "All NE Neighbours tests passed :)" << std::endl;
}

void BuildGraph(std::vector<std::vector<std::pair<int, double>>>& g) {
  g.clear();
  g.resize(all_points.size());
  double delta_angle = M_PI / 8.0;
  double angle = 0.0;
  for (int i = 0; i < 16; ++i) {
    // Rotate all points
    angle += delta_angle;
    auto tpoints = all_points;
    for (auto& point : all_points) {
      double x = point.x;
      double y = point.y;
      point.x = x * cosl(angle) - y * sinl(angle);
      point.y = x * sinl(angle) + y * cosl(angle);
    }
    std::vector<int> points(all_points.size());
    for (int i = 0; i < points.size(); ++i) {
      points[i] = i;
    }
    std::sort(points.begin(), points.end(), [](int p1, int p2) {
      return all_points[p1].x < all_points[p2].x;
    });
    all_neighbours.assign(all_points.size(), -1);
    FindNENeighbours(points, 0, all_points.size());
    all_points = tpoints;
    for (int i = 0; i < all_points.size(); ++i) {
      int neighbour_id = all_neighbours[i];
      if (neighbour_id < 0) continue;
      bool already_connected = false;
      for (auto& e : g[i]) {
        if (e.first == neighbour_id) {
          already_connected = true;
          break;
        }
      }
      if (already_connected) continue;
      double dist = ManhattanDistance(all_points[i], all_points[neighbour_id]);
      g[i].push_back(std::make_pair(neighbour_id, dist));
      g[neighbour_id].push_back(std::make_pair(i, dist));
    }
  }
  for (int i = 0; i < g.size(); ++i) {
    assert(g[i].size() > 0);
  }
}

void Prim(const std::vector<std::vector<std::pair<int, double>>>& g,
          std::vector<int>& parents) {
  parents.assign(g.size(), -1);
  std::set<std::pair<double, int>> s;
  std::vector<double> dists(g.size());
  dists[0] = 0;
  s.insert(std::make_pair(dists[0], 0));
  for (int i = 1; i < g.size(); ++i) {
    dists[i] = std::numeric_limits<double>::max();
    s.insert(std::make_pair(dists[i], i));
  }
  while (!s.empty()) {
    int u = s.begin()->second;
    s.erase(s.begin());
    for (int i = 0; i < g[u].size(); ++i) {
      int to = g[u][i].first;
      if (dists[to] < g[u][i].second) continue;
      auto p = std::make_pair(dists[to], to);
      auto it = s.find(p);
      if (it != s.end()) {
        s.erase(it);
        dists[to] = g[u][i].second;
        s.insert(std::make_pair(g[u][i].second, to));
        parents[to] = u;
      }
    }
  }
  for (int i = 1; i < parents.size(); ++i) {
    assert(parents[i] >= 0);
  }
}

double GetTreeWeight(const std::vector<int>& parents) {
  double total_weight = 0.0;
  for (int i = 0; i < parents.size(); ++i) {
    if (parents[i] >= 0) {
      total_weight += ManhattanDistance(all_points[i], all_points[parents[i]]);
    }
  }
  return total_weight;
}

void BrutePrim() {
  std::vector<std::vector<std::pair<int, double>>> g(all_points.size());
  for (int i = 0; i < all_points.size(); ++i) {
    for (int j = 0; j < all_points.size(); ++j) {
      if (i == j) continue;
      double dist = ManhattanDistance(all_points[i], all_points[j]);
      g[i].push_back(std::make_pair(j, dist));
      g[j].push_back(std::make_pair(i, dist));
    }
  }
  std::vector<int> parents;
  double total_weight;
  Prim(g, parents);
  std::ofstream tout("tout1.txt");
  for (int i = 0; i < parents.size(); ++i) {
    tout << parents[i] << std::endl;
  }
  std::cout << GetTreeWeight(parents) << std::endl;
}

void ManhattanMST() {
  std::vector<std::vector<std::pair<int, double>>> g;
  std::vector<int> parents;
  double total_weight;
  BuildGraph(g);
  std::ofstream outg("outg.txt");
  for (int i = 0; i < g.size(); ++i) {
    for (int j = 0; j < g[i].size(); ++j) {
      outg << g[i][j].first << " ";
    }
    outg << std::endl;
  }
  Prim(g, parents);
  std::ofstream tout("tout2.txt");
  for (int i = 0; i < parents.size(); ++i) {
    tout << parents[i] << std::endl;
  }
  std::cout << "Fast: " << GetTreeWeight(parents) << std::endl;
}

int main(int argc, char** argv) {
  assert(argc > 1);
  int n;
  std::ifstream in(argv[1]);
  std::ofstream out("output.txt");
  in >> n;
  std::cout << n << std::endl;
  all_points.resize(n);
  for (int i = 0; i < n; ++i) {
    in >> all_points[i];
  }
  std::cout << "All points are read!" << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  ManhattanMST();
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
  return 0;
}