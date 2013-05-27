#include "unit.h"


CUnit::CUnit() {
	for (int i=0;i<CDevice::MAX_PARAMS;i++) {
		param_values[i] = 0.0f;
	}
}

CUnit::~CUnit() {

}

