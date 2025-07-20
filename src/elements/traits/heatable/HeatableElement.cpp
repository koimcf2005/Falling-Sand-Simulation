#include "src/elements/traits/heatable/HeatableElement.hpp"

bool HeatableElement::handleHeat(IMatrix& matrix) {
	// Trigger heat reaction if temperature exceeds threshold and not already triggered
	if (m_ShouldReactToHeat && m_Temperature >= m_TemperatureThreshold) {
		reactToHeat(matrix);
		m_ShouldReactToHeat = false;
		return true;
	}
	// Trigger cooling reaction if previously heated and now cooled
	else if (!m_ShouldReactToHeat && m_Temperature < m_TemperatureThreshold) {
		reactToCooling(matrix);
		m_ShouldReactToHeat = true;
		return true;
	}
	return false;
}

float HeatableElement::getTemperature() const { return m_Temperature; }
void HeatableElement::setTemperature(float temperature) { m_Temperature = temperature; }
void HeatableElement::addTemperature(float temperature) { m_Temperature += temperature; }
float HeatableElement::getTemperatureThreshold() const { return m_TemperatureThreshold; }
