#pragma once

namespace network
{
	namespace event
	{
		/*
		* @brief An event template.
		*/
		class Event
		{
		public:
			/*
			* @return whether (or not) an event is of the given type.
			*/
			template<class M> bool is() const
			{
				return type == M::STATIC_TYPE;
			}
			/*
			* @return the event as one of the given type.
			*/
			template<class M> const M* as() const
			{
				return static_cast<const M*>(this);
			}

		protected:
			enum class Type
			{
				Connection,
				Disconnection,
				Exchange
			};

			Event(Type type) : type(type)
			{
			}

		private:
			Type type;

		};
	}
}
