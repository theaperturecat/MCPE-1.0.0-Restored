/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "common_header.h"

#include "util/ClassID.h"

ClassID::IDType ClassID::getNextID() {
	static IDType lastID = 0;
	return ++lastID;
}
