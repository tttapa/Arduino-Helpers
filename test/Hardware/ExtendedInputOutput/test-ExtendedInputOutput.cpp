#include <gmock-wrapper.h>

#include <Hardware/ExtendedInputOutput/ExtendedInputOutput.hpp>
#include <type_traits>

using namespace ::testing;
USING_AH_NAMESPACE;
using namespace ExtIO;

W_SUGGEST_OVERRIDE_OFF

class MockExtIOElement : public ExtendedIOElement {
  public:
    MockExtIOElement(pin_t length) : ExtendedIOElement(length) {}

    MOCK_METHOD2(pinMode, void(pin_t, uint8_t));
    MOCK_METHOD2(digitalWrite, void(pin_t, uint8_t));
    MOCK_METHOD1(digitalRead, int(pin_t));
    MOCK_METHOD1(analogRead, analog_t(pin_t));
    MOCK_METHOD2(analogWrite, void(pin_t, analog_t));

    MOCK_METHOD0(begin, void());
    MOCK_METHOD0(update, void());
};

W_SUGGEST_OVERRIDE_ON

TEST(ExtendedInputOutput, digitalRead) {
    MockExtIOElement el_1 = {10};
    MockExtIOElement el_2 = {10};

    InSequence seq;

    EXPECT_CALL(ArduinoMock::getInstance(), digitalRead(0));
    digitalRead(0);

    EXPECT_CALL(el_1, digitalRead(0));
    digitalRead(el_1.pin(0));
    EXPECT_CALL(el_1, digitalRead(0));
    digitalRead((int)el_1.pin(0));
    EXPECT_CALL(el_1, digitalRead(9));
    digitalRead(el_1.pin(9));
    EXPECT_CALL(el_1, digitalRead(9));
    digitalRead((int)el_1.pin(9));

    EXPECT_CALL(el_2, digitalRead(0));
    digitalRead(el_2.pin(0));
    EXPECT_CALL(el_2, digitalRead(0));
    digitalRead((int)el_2.pin(0));
    EXPECT_CALL(el_2, digitalRead(9));
    digitalRead(el_2.pin(9));
    EXPECT_CALL(el_2, digitalRead(9));
    digitalRead((int)el_2.pin(9));

    Mock::VerifyAndClear(&ArduinoMock::getInstance());
}

TEST(ExtendedInputOutput, pinOutOfBoundsGt) {
    MockExtIOElement el = {10};
    el.pin(9);
    try {
        el.pin(10);
        FAIL();
    } catch (ErrorException &e) {
        EXPECT_EQ(e.getErrorCode(), 0x4567);
    }
}

TEST(ExtendedInputOutput, pinOutOfBoundsLt) {
    MockExtIOElement el = {10};
    el.pin(0);
    try {
        el.pin(-1);
        FAIL();
    } catch (ErrorException &e) {
        EXPECT_EQ(e.getErrorCode(), 0x4567);
    }
}

TEST(ExtendedInputOutput, nonExistentPin) {
    MockExtIOElement el = {10};
    pin_t pin = el.pin(9) + 1;
    try {
        digitalRead(pin);
        FAIL();
    } catch (ErrorException &e) {
        EXPECT_EQ(e.getErrorCode(), 0x8888);
    }
}

TEST(ExtendedInputOutput, nonExistentPinOfDeletedElement) {
    MockExtIOElement el_1 = {10};
    pin_t pin;
    {
        MockExtIOElement el_2 = {10};
        pin = el_2.pin(0);
    }
    MockExtIOElement el_3 = {10};
    try {
        digitalRead(pin);
        FAIL();
    } catch (ErrorException &e) {
        EXPECT_EQ(e.getErrorCode(), 0x8888);
    }
}

TEST(ExtendedInputOutput, outOfPinNumbers) {
    pin_t max = std::numeric_limits<pin_t>::max();
    try {
        MockExtIOElement el_1 = {max};
        FAIL();
    } catch (ErrorException &e) {
        EXPECT_EQ(e.getErrorCode(), 0x00FF);
    }
}