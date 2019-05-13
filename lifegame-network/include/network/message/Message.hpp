#pragma once

namespace network
{
	namespace message
	{
		/*
		* @brief A message template.
		*/
		class Message
		{
		public:
			/*
			* @return whether (or not) a message is of the given type.
			*/
			template<class M> bool is() const
			{
				return type == M::STATIC_TYPE;
			}
			/*
			* @return the message as one of the given type.
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
				Raw
			};

			Message(Type type) : type(type)
			{
			}

		private:
			Type type;

		};
	}
}
