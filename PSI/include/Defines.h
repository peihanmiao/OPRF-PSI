#pragma once

#include <cryptoTools/Network/IOService.h>
#include <cryptoTools/Network/Endpoint.h>
#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Crypto/PRNG.h>
#include <cryptoTools/Crypto/Curve.h>
#include <cryptoTools/Crypto/RandomOracle.h>
#include <cryptoTools/Common/BitVector.h>
#include <cryptoTools/Common/Timer.h>
#include <libOTe/TwoChooseOne/IknpOtExtSender.h>
#include <libOTe/TwoChooseOne/IknpOtExtReceiver.h>


namespace PSI {

	using i64 = oc::i64;
	using u64 = oc::u64;
	using i32 = oc::i32;
	using u32 = oc::u32;
	using i16 = oc::i16;
	using u16 = oc::u16;
	using i8 = oc::i8;
	using u8 = oc::u8;
	using block = oc::block;

	using BitVector = oc::BitVector;

	template<typename T> using span = oc::span<T>;
	
	using PRNG = oc::PRNG;
	using AES = oc::AES;
	using AESDec = oc::AESDec;
	using RandomOracle = oc::RandomOracle;

	using IknpOtExtSender = oc::IknpOtExtSender;
	using IknpOtExtReceiver = oc::IknpOtExtReceiver;

	using IOService = oc::IOService;
	using Endpoint = oc::Endpoint;
	using Channel = oc::Channel;
	using EpMode = oc::EpMode;
	
	using TimeUnit = oc::Timer::timeUnit;
	using Timer = oc::Timer;

	
}
