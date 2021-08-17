#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace rv
{
	typedef uint32_t Entity;
	static constexpr Entity InvalidEntity = UINT32_MAX;

	struct EntityProxy
	{
		/**
		 * @brief The Unique ID of this Entity, matches it's position in the Entity Registry.
		 */
		Entity uniqueId = InvalidEntity;

		/**
		 * @brief The Group ID of this Entity, it is relative to it's archtype group position,
		 * all components of this entity will be stored at the same relative group position id.
		 */
		int32_t groupId;

		/**
		 * @brief Either or not this proxy is being tracked by the registry.
		 */
		constexpr bool IsTracked() { return uniqueId != InvalidEntity; }
	};

	struct EntityReg
	{
		/**
		 * @brief The Unique ID of this Entity, matches it's position in the Entity Registry.
		 */
		Entity uniqueId;
		/**
		 * @brief The Group ID of this Entity, it is relative to it's archtype group position,
		 * all components of this entity will be stored at the same relative group position id.
		 */
		int32_t groupId;
		/**
		 * @brief Amount of component types registered for this Entity.
		 */
		int32_t typesCount;
		/**
		 * @brief Array that holds the component types.
		 */
		intptr_t* compTypes;

		constexpr EntityReg() : uniqueId(InvalidEntity), groupId(-1), typesCount(0), compTypes(nullptr) {}

		EntityReg(const EntityReg& other)
		    : uniqueId(other.uniqueId), groupId(other.groupId), typesCount(other.typesCount)
		{
			compTypes = new intptr_t[typesCount];
			memcpy(compTypes, other.compTypes, sizeof(intptr_t) * typesCount);
		}

		EntityReg(EntityReg&& other)
		    : uniqueId(other.uniqueId), groupId(other.groupId), typesCount(other.typesCount),
		      compTypes(other.compTypes)
		{
			other.uniqueId = InvalidEntity;
			other.groupId = -1;
			other.typesCount = 0;
			other.compTypes = nullptr;
		}

		~EntityReg()
		{
			uniqueId = InvalidEntity;
			groupId = -1;
			typesCount = 0;
			delete[] compTypes;
		}

		void print() { fprintf(stdout, "Entity(%i)", uniqueId); }
	};

} // namespace rv

#endif