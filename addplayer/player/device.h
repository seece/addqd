#ifndef DEVICE_H_
#define DEVICE_H_

class CDevice {
	public:
		static const int MAX_PARAMS = 10;

		CDevice();
		virtual ~CDevice();
		void init();
		virtual int getParamNum();
	private:

};

#endif