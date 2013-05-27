#ifndef DEVICE_H_
#define DEVICE_H_

class CDevice {
	public:
		static const int MAX_PARAMS = 10;

		CDevice();
		~CDevice();
		void init();
		int getParams();
	private:
		int num_params;
};

#endif