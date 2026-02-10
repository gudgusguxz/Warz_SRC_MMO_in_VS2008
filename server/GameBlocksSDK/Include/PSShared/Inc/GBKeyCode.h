// cross-platform virtual key codes.  map to platform's virtual key code.

#ifndef GB_VK_CODE_H
#define GB_VK_CODE_H

namespace GameBlocks {

// for now the below match windows VK_* key codes exactly, so
// no conversion is necessary if you use those values.
enum EGBKeycode {
	GB_VK_UNKNOWN = 0,
	GB_VK_ESCAPE = '\033',
	GB_VK_TAB = '\t',
	GB_VK_DELETE = 0x2e,
	GB_VK_INSERT = 0x2d,
	GB_VK_F1 = 0x70,
	GB_VK_F2 = 0x71,
	GB_VK_F3 = 0x72,
	GB_VK_F4 = 0x73,
	GB_VK_F5 = 0x74,
	GB_VK_F6 = 0x75,
	GB_VK_F7 = 0x76,
	GB_VK_F8 = 0x77,
	GB_VK_F9 = 0x78,
	GB_VK_F10 = 0x79,
	GB_VK_F11 = 0x7A,
	GB_VK_F12 = 0x7B,
	GB_VK_0 = 0x30,
	GB_VK_1 = 0x31,
	GB_VK_2 = 0x32,
	GB_VK_3 = 0x33,
	GB_VK_4 = 0x34,
	GB_VK_5 = 0x35,
	GB_VK_6 = 0x36,
	GB_VK_7 = 0x37,
	GB_VK_8 = 0x38,
	GB_VK_9 = 0x39,
	GB_VK_BACKSPACE = 0x08,
	GB_VK_RETURN = 0X0D,
	GB_VK_LEFT = 0X25,
	GB_VK_RIGHT = 0X27,
	GB_VK_DOWN = 0X28,
	GB_VK_UP = 0X26,
};

} // namespace GameBlocks 

#endif // GB_VK_CODE_H
