//
//This file contains all of the command code definitions for this application
//NOTE: This file is used on both C++ compilations along with MIDL compilations
//		Therefore, be aware of what gets put in here so both compilers are able
//		to compile this file without error or warnings
//		Several enumeration values are removed from being defined during MIDL
//		compilations.  The reason is due to the fact that they should not be
//		accessed from this env.
//

#ifndef __EXPOSE_TO_MIDL
#pragma once
namespace Canberra {
	namespace DataTypes {
		namespace Application {
			//
			//This enumeration is used to define all possible
			//Parameter attributes
			//
			namespace ParameterAttributes {
#endif

				//Parameter attributes
				typedef enum AttributesTag {
					ReadOnly=1,				//Parameter is read-only
					HardwareWrite=2,		//Parameter is written to hardware
					Array=4,				//Parameter is an array type of parameter
					PerInput=8,				//Parameter is a per input parameter (input 1-N)
					RealTime=16,			//Parameter is a real time parameter
					Required=32,			//Parameter is required
					Enumerable=64,			//Parameter has descrete values instead of [min, max]
				} Attributes;

				//Types of parameter values
				typedef enum ValueTypeTag {
					UnknownType =	0,
					UbyteType =		1,
					UshortType =	2,
					UlongType =		3,
					ByteType =		4,
					ShortType =		5,
					LongType =		6,
					FloatType =		7,
					DoubleType =	8,
					WstringType =	9,
					Int64Type =		10,
					Uint64Type =	11,
					BoolType =		12,
					DateTimeType =	13,
					NullType =		14,
				} ValueType;
#ifndef __EXPOSE_TO_MIDL
			}
		}
	}
}
#endif
