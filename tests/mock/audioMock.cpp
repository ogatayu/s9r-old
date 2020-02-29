#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "audio.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class mock_AudioCtrl : public AudioCtrl
{
public:
    MOCK_METHOD0(Create, AudioCtrl*());
};
