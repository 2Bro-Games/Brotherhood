#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "Layer.h"
#include <vector>

namespace Brotherhood {
	class LayerStack {
	public:
		using LayerStack_t = std::vector<std::shared_ptr<Layer>>;

		LayerStack() = default;
		~LayerStack();

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> overlay);
		void PopLayer(std::shared_ptr<Layer> layer);
		void PopOverlay(std::shared_ptr<Layer> overlay);

		constexpr LayerStack_t::iterator begin() noexcept { return m_Layers.begin(); }
		constexpr LayerStack_t::iterator end() noexcept { return m_Layers.end(); }
		constexpr LayerStack_t::reverse_iterator rbegin() noexcept { return m_Layers.rbegin(); }
		constexpr LayerStack_t::reverse_iterator rend() noexcept { return m_Layers.rend(); }

		constexpr LayerStack_t::const_iterator begin() const noexcept { return m_Layers.begin(); }
		constexpr LayerStack_t::const_iterator end() const noexcept { return m_Layers.end(); }
		constexpr LayerStack_t::const_reverse_iterator rbegin() const noexcept { return m_Layers.rbegin(); }
		constexpr LayerStack_t::const_reverse_iterator rend() const noexcept { return m_Layers.rend(); }

	private:
		LayerStack_t m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}

#endif // !LAYER_STACK_H
