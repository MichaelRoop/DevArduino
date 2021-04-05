// 
// 
// 

#include "MsgMessages.h"


// Implement specialization
template<> MsgBinary<float>::MsgBinary() {
	Size = sizeof(MsgBinary<float>);
}

