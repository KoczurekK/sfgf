#ifndef POLYGON_HPP
#define POLYGON_HPP

/* =========================================================== *
 * SFGF (c) Kamil Koczurek | koczurekk@gmail.com               *
 * GNU GPL v3 License http://www.gnu.org/licenses/gpl-3.0.html *
 * =========================================================== */

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include "GameObject.hpp"
#include "Collider.hpp"

namespace sfgf {
	class Polygon
		: public GameObject
		, public sf::Drawable {
	private:
		Collider sample_collider;
		Collider tranformed_collider;

		std::vector<sf::Vertex> m_arr;
		const sf::Texture *m_tx = nullptr;

		virtual void draw(sf::RenderTarget& window, sf::RenderStates states) const {
			states.transform *= getTransform();
			states.texture = m_tx;

			window.draw(m_arr.data(), 4, sf::TriangleFan, states);
		}

	public:
		Polygon(size_t size);

		void setTexture(const sf::Texture& tex, const std::vector<sf::Vector2f>& texCorrds);

		void setColor(sf::Color c);

		void setVertices(const std::vector<sf::Vector2f>& vertices);
		const std::vector<sf::Vertex>& getVertices() const;

		void setSampleCollider(const Collider& c);
		const Collider &getSampleCollider() const;
		const Collider &getTransformedCollider() const;
		Collider getDefaultSampleCollider() const;

		void updateCollider();
		virtual void update(sf::Time);

		bool contains(sf::Vector2f point) const;
		bool intersects(Polygon& poly) const;
		bool collides(Polygon& poly) const;

		sf::FloatRect getGlobalBounds() const;
	};

	Polygon::Polygon(size_t size) {
		m_arr.resize(size);
	}

	void Polygon::setTexture(const sf::Texture& tex, const std::vector<sf::Vector2f>& texCorrds) {
		for(auto i = 0u; i < texCorrds.size(); ++i) {
			m_arr[i].texCoords = texCorrds[i];
		}

		m_tx = &tex;
	}

	void Polygon::setColor(sf::Color c) {
		for(auto& v: m_arr) {
			v.color = c;
		}
	}

	void Polygon::setVertices(const std::vector<sf::Vector2f>& vertices) {
		for(auto i = 0u; i < vertices.size(); ++i) {
			m_arr[i].position = vertices[i];
		}

		setSampleCollider(getDefaultSampleCollider());
	}
	const std::vector<sf::Vertex>& Polygon::getVertices() const {
		return m_arr;
	}

	void Polygon::setSampleCollider(const Collider& c) {
		sample_collider = c;
		tranformed_collider = c;
	}
	const Collider& Polygon::getSampleCollider() const {
		return sample_collider;
	}
	const Collider& Polygon::getTransformedCollider() const {
		return tranformed_collider;
	}
	Collider Polygon::getDefaultSampleCollider() const {
		Collider result;
		for(const sf::Vertex& v: m_arr) {
			result.pushBack(v.position);
		}

		return result;
	}

	void Polygon::updateCollider() {
		tranformed_collider = sample_collider;
		tranformed_collider.applyTransform(getTransform());
	}
	void Polygon::update(sf::Time) {
		updateCollider();
	}

	bool Polygon::intersects(Polygon& poly) const {
		return tranformed_collider.intersects(poly.getTransformedCollider());
	}
	bool Polygon::collides(Polygon& poly) const {
		return tranformed_collider.collides(poly.getTransformedCollider());
	}
	bool Polygon::contains(sf::Vector2f point) const {
		return tranformed_collider.contains(point);
	}

	sf::FloatRect Polygon::getGlobalBounds() const {
		return tranformed_collider.getGlobalBounds();
	}
}

#endif // POLYGON_HPP
