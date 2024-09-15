#include "LayerStack.h"

Brotherhood::LayerStack::~LayerStack() {
	for (auto layer : m_Layers) {
		layer->OnDetach();
	}
	m_Layers.clear();
}

void Brotherhood::LayerStack::PushLayer(std::shared_ptr<Layer> layer) {
	m_Layers.emplace(begin() + m_LayerInsertIndex++, layer);
	layer->OnAttach();
}

void Brotherhood::LayerStack::PushOverlay(std::shared_ptr<Layer> overlay) {
	m_Layers.emplace_back(overlay);
	overlay->OnAttach();
}

void Brotherhood::LayerStack::PopLayer(std::shared_ptr<Layer> layer) {
	auto it = std::find(begin(), begin() + m_LayerInsertIndex, layer);
	if (it != end()) {
		layer->OnDetach();
		m_Layers.erase(it);
		--m_LayerInsertIndex;
	}
}

void Brotherhood::LayerStack::PopOverlay(std::shared_ptr<Layer> overlay) {
	auto it = std::find(begin() + m_LayerInsertIndex, end(), overlay);
	if (it != end()) {
		overlay->OnDetach();
		m_Layers.erase(it);
	}
}