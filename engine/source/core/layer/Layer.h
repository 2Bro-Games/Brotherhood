#ifndef LAYER_H
#define LAYER_H

#include "core/event/Event.h"

namespace Brotherhood {
	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(/*float ts*/) {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}

#endif // !LAYER_H