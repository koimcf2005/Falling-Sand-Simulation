#ifndef HEATABLE_ELEMENT_HPP
#define HEATABLE_ELEMENT_HPP

#include "src/core/IMatrix.hpp"

/**
 * @brief Interface for elements that respond to temperature changes.
 * 
 * Provides basic temperature tracking and an interface for triggering heat or cooling reactions.
 * Intended to be inherited by elements that should react when reaching specific temperature thresholds.
 */
class HeatableElement {
public:
	/**
	 * @brief Get the current temperature of the element.
	 * @return The element's temperature in degrees Celsius.
	 */
	float getTemperature() const;

	/**
	 * @brief Set the element's temperature.
	 * @param temperature The new temperature to assign.
	 */
	void setTemperature(float temperature);

	/**
	 * @brief Increment the element's temperature by a delta.
	 * @param temperature The amount to add to the current temperature.
	 */
	void addTemperature(float temperature);

	/**
	 * @brief Get the temperature threshold for triggering heat/cooling reactions.
	 * @return The threshold temperature.
	 */
	float getTemperatureThreshold() const;

protected:
	/**
	 * @brief Checks whether the element should react to heat or cooling.
	 * 
	 * Triggers `reactToHeat()` if temperature exceeds the threshold, or `reactToCooling()` if it falls below.
	 * @param matrix A reference to the simulation matrix.
	 * @return true if a reaction occurred; false otherwise.
	 */
	bool handleHeat(IMatrix& matrix);

	/**
	 * @brief React to heat exceeding the threshold.
	 * 
	 * Subclasses must implement this to define what happens when overheated.
	 */
	virtual void reactToHeat(IMatrix& matrix) = 0;

	/**
	 * @brief React to cooling below the threshold.
	 * 
	 * Subclasses must implement this to define what happens when temperature drops.
	 */
	virtual void reactToCooling(IMatrix& matrix) = 0;

	/// The current temperature of the element.
	float m_Temperature = 20.0f;

	/// The temperature threshold to trigger heating/cooling reactions.
	float m_TemperatureThreshold = 100.0f;

	/// Whether the element is currently allowed to react to heat.
	bool m_ShouldReactToHeat = true;
};

#endif // HEATABLE_ELEMENT_HPP
