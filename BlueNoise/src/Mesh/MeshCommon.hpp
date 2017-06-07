/*
 * MeshCommon.hpp
 *
 * define some common stuff for Mesh classes
 * 
 * Li-Yi Wei
 * 4/4/01
 * 10/30/07 modified
 */

#ifndef _MESH_COMMON_HPP
#define _MESH_COMMON_HPP

template<class T, class U>
T my_dynamic_cast(U object)
{
#if 0
    return dynamic_cast<T>(object);
#else
    return reinterpret_cast<T>(object);
#endif
}

#endif
