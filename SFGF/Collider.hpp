#ifndef COLLIDER_HPP
#define COLLIDER_HPP

/* =========================================================== *
 * SFGF (c) Kamil Koczurek | koczurekk@gmail.com               *
 * GNU GPL v3 License http://www.gnu.org/licenses/gpl-3.0.html *
 * =========================================================== */

//! @file
//! \brief Collider class implementation

#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <vector>

//! \namespace sfgf
//! \brief Contains all SFGF classes
namespace sfgf {
	//! \class Collider
	//! \brief Handles collisions
	//!
	//! Contains basic set of points which can be later transformed to represent bounds of an object.
	//! Also supports globalBounds to make collision detection more efficient. <br />
	//! Sample code:
	//! \code{.cpp}
	//! auto circle = sfgf::Collider::circle(5); // Make circle, r = 5
	//! auto rect = sfgf::Collider::rectangle({10, 10}); // Make rectangle (10, 10)
	//!
	//! auto t = sf::Transform().translate({9, 0}); // Make transform translating by 9 to the right
	//! circle.applyTransform(t); // Apply that transform to circle
	//!
	//! std::cout << std::boolalpha << circle.collides(rect) << std::endl; //True
	//! circle.applyTransform(t); // Apply transform again
	//! std::cout << std::boolalpha << circle.collides(rect) << std::endl; //False
	//! \endcode
	class Collider {
		std::vector<sf::Vector2f> m_arr;
		sf::FloatRect m_globalBounds;

		static bool nonnegatively_dimensional_rect_intersection(sf::FloatRect lhs, sf::FloatRect rhs) {
			lhs.top = std::abs(lhs.top);
			lhs.left = std::abs(lhs.left);

			rhs.top = std::abs(rhs.top);
			rhs.left = std::abs(rhs.left);

			if(lhs.left > rhs.left + rhs.width) {
				return false;
			}
			if(lhs.left + lhs.width < rhs.left) {
				return false;
			}
			if(lhs.top > rhs.top + rhs.height) {
				return false;
			}
			if(lhs.top + lhs.height < rhs.top) {
				return false;
			}

			return true;
		}

		void updateGlobalBounds() {
			sf::Vector2f max {0, 0};
			sf::Vector2f pos {m_arr.empty() ? sf::Vector2f{0, 0} : m_arr[0]};

			for(const sf::Vector2f& pt: m_arr) {
				max.x = pt.x > max.x ? pt.x : max.x;
				max.y = pt.y > max.y ? pt.y : max.y;

				pos.x = pt.x < pos.x ? pt.x : pos.x;
				pos.y = pt.y < pos.y ? pt.y : pos.y;
			}

			m_globalBounds.top = pos.y;
			m_globalBounds.left = pos.x;
			m_globalBounds.width = max.x - pos.x;
			m_globalBounds.height = max.y - pos.y;
		}

	public:
		//! \brief Returns collider in shape of circle
		//! \param [in] radius – radius of circle
		//! \param [in] cnt – count of points to make
		//!
		//! \returns new collider
		//!
		//! \code{.cpp}
		//! somePolygon.setSampleCollider(sfgf::Collider::circle(7));
		//! \endcode
		static Collider circle(float radius, size_t cnt = 128);

		//! \brief Returns collider in shape of rectangle
		//! \param [in] size – dimensions of rectangle
		//!
		//! \returns new collider
		//!
		//! \code{.cpp}
		//! somePolygon.setSampleCollider(sfgf::Collider::rectangle({20, 10}));
		//! \endcode
		static Collider rectangle(sf::Vector2f size);

		//! \brief Check two lines for intersection
		//! \param [in] p1 – beginning of line I
		//! \param [in] q1 – end of line I
		//! \param [in] p2 – beginning of line II
		//! \param [in] q2 – end of line II
		//!
		//! \returns True if lines intersect, False otherwise
		//!
		//! \code{.cpp}
		//! std::cout << std::boolapha << sfgf::Collider::lineIntersection({0, 0}, {50, 20}, {25, 0}, {25 50}) << std::endl;
		//! \endcode
		static bool lineIntersection(sf::Vector2f p1, sf::Vector2f q1, sf::Vector2f p2, sf::Vector2f q2);

		//! \brief Applies transform
		//! \param [in] t – transform to use
		//!
		//! Used to adjust position/rotation/scale of collider to actual object.<br />
		//! Sample code:
		//! \code{.cpp}
		//! this->collider.applyTransform(this->polygon.getTransform());
		//! \endcode
		void applyTransform(const sf::Transform& t);

		//! \brief Adds point to internal array
		//! \param [in] pt – coordinates in local system
		//!
		//! Can be used to create collider in non–standard shape, aka custom polygon. <br />
		//! Sample code:
		//! \code{.cpp}
		//! while(inputFile >> x >> y) {
		//!		collider.pushBack({x, y});
		//! }
		//! \endcode
		void pushBack(sf::Vector2f pt);

		//! \brief Clears points
		//!
		//! Simply clears internal array (std::vector), empty collider never detects any collision.
		void clear();

		//! \brief Returns global bounds
		//!
		//! Get the global bounding rectangle of the entity. <br />
		//! The returned rectangle is in global coordinates, which means that it takes in account the transformations (translation, rotation, scale, …) that are applied to the entity.
		//! In other words, this function returns the bounds of the collider in the global 2D world's coordinate system.
		//!
		//! \returns Global bounding rectangle of the entity
		sf::FloatRect getGlobalBounds() const {
			return m_globalBounds;
		}

		//! \brief Check for intersection
		//! \param [in] poly – collider to check
		//!
		//! Tests if any side of (*this) intersects with any side of poly.
		//!
		//! \returns True if colliders intersect, False otherwise
		bool intersects(const Collider& poly) const;

		//! \brief Check if point is inside of collider
		//! \param [in] point – point to check
		//!
		//! Tests if given point is inside of (*this).
		//!
		//! \returns True if point is in collider, false otherwise
		bool contains(sf::Vector2f point) const;

		//! \brief Check if two colliders collide
		//! \param [in] poly – collider to check against
		//!
		//! Tests whether colliders collide or not. They do, if any of theirs side intersect or if no point of one collider is outside of another.
		//!
		//! \returns True if they collide, False otherwise.
		bool collides(const Collider& poly) const;
	};

	Collider Collider::circle(float radius, size_t cnt) {
		Collider result;
		for(auto i = 0u; i < cnt; ++i) {
			result.pushBack({
				std::sin(6.28 / cnt * i) * radius + radius,
				std::cos(6.28 / cnt * i) * radius + radius
			});
		}

		return result;
	}
	Collider Collider::rectangle(sf::Vector2f size) {
		Collider result;
		result.pushBack({0, 0});
		result.pushBack({size.x, 0});
		result.pushBack(size);
		result.pushBack({0, size.y});

		return result;
	}

	bool Collider::lineIntersection(sf::Vector2f p1, sf::Vector2f q1, sf::Vector2f p2, sf::Vector2f q2) {
		auto isOnSegment = [](sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
			return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
					q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
		};
		auto orientation = [](sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
			int val = (q.y - p.y) * (r.x - q.x) -
					  (q.x - p.x) * (r.y - q.y);

			if (val == 0) {
				return 0;
			}

			return (val > 0) ? 1 : 2;
		};

		// Find the four orientations needed for general and
		// special cases
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		// General case
		if (o1 != o2 && o3 != o4)
			return true;

		// Special Cases
		if ((o1 == 0 && isOnSegment(p1, p2, q1))
		||  (o2 == 0 && isOnSegment(p1, q2, q1))
		||  (o3 == 0 && isOnSegment(p2, p1, q2))
		||  (o4 == 0 && isOnSegment(p2, q1, q2))) {
			return true;
		}

		return false;
	}

	void Collider::applyTransform(const sf::Transform& t) {
		std::transform(
			m_arr.begin(),
			m_arr.end(),
			m_arr.begin(),
			[&t](sf::Vector2f p) {
				return t.transformPoint(p);
			}
		);

		updateGlobalBounds();
	}
	void Collider::pushBack(sf::Vector2f pt) {
		m_arr.push_back(pt);
		updateGlobalBounds();
	}
	void Collider::clear() {
		m_arr.clear();
		updateGlobalBounds();
	}

	bool Collider::intersects(const Collider& poly) const {
		if(!nonnegatively_dimensional_rect_intersection(getGlobalBounds(), poly.getGlobalBounds())
		|| poly.m_arr.empty()
		|| m_arr.empty()) {
			return false;
		}

		auto arr = m_arr;
		arr.push_back(arr.front());

		auto poly_arr = poly.m_arr;
		poly_arr.push_back(poly_arr.front());

		for(auto i = 1u; i < arr.size(); ++i) {
			auto p1 = arr[i - 1], q1 = arr[i];

			for(auto j = 1u; j < poly_arr.size(); ++j) {
				auto p2 = poly_arr[j - 1], q2 = poly_arr[j];

				if(lineIntersection(p1, q1, p2, q2)) {
					return true;
				}
			}
		}

		return false;
	}
	bool Collider::contains(sf::Vector2f point) const {
		int i, j, nvert = m_arr.size();
		bool c = false;

		for(i = 0, j = nvert - 1; i < nvert; j = i++) {
			auto pti = m_arr[i];
			auto ptj = m_arr[j];

			if(((pti.y >= point.y) != (ptj.y >= point.y)) &&
				(point.x <= (ptj.x - pti.x) * (point.y - pti.y) / (ptj.y - pti.y) + pti.x)) {
				c = !c;
			}
		  }

		  return c;
	}

	bool Collider::collides(const Collider& poly) const {
		if(!getGlobalBounds().intersects(poly.getGlobalBounds())) {
			return false;
		}

		if(intersects(poly)) {
			return true;
		}

		for(auto v: poly.m_arr) {
			if(!contains(v)) {
				return false;
			}
		}

		return true;
	}
}

#endif // COLLIDER_HPP
