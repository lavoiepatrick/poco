//
// NetworkInterfaceTest.cpp
//
// $Id: //poco/1.4/Net/testsuite/src/NetworkInterfaceTest.cpp#1 $
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "NetworkInterfaceTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/IPAddress.h"
#include <iostream>
#include <iomanip>


using Poco::Net::NetworkInterface;
using Poco::Net::IPAddress;
using Poco::NotFoundException;


NetworkInterfaceTest::NetworkInterfaceTest(const std::string& name): CppUnit::TestCase(name)
{
}


NetworkInterfaceTest::~NetworkInterfaceTest()
{
}


void NetworkInterfaceTest::testMap()
{
	NetworkInterface::Map m = NetworkInterface::map(false, false);
	assert (!m.empty());
	for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << std::endl << "=============" << std::endl;

		std::cout << "Index:       " << it->second.index() << std::endl;
		std::cout << "Name:        " << it->second.name() << std::endl;
		std::cout << "DisplayName: " << it->second.displayName() << std::endl;
		std::cout << "Status: " << (it->second.isUp() ? "Up" : "Down") << std::endl;

		NetworkInterface::MacAddress mac(it->second.macAddress());
		if (!mac.empty()) {
			std::cout << "Mac Address: (" << it->second.hwType() << ")";
			for (unsigned i = 0; i < mac.size(); ++i)
				std::cout << ((i == 0) ? ' ' : ':') << std::hex << std::setw(2) << std::setfill('0') << (unsigned)mac[i];
			std::cout << std::dec << std::endl;
		}

		typedef NetworkInterface::AddressList List;
		const List& ipList = it->second.addressList();
		List::const_iterator ipIt = ipList.begin();
		List::const_iterator ipEnd = ipList.end();
		for (int counter = 0; ipIt != ipEnd; ++ipIt, ++counter)
		{
			std::cout << std::endl << "----------" << std::endl;
			std::cout << "Address " << counter << std::endl;
			std::cout << "----------" << std::endl;
			std::cout << "Address:     " << ipIt->get<NetworkInterface::IP_ADDRESS>().toString() << std::endl;
			IPAddress addr = ipIt->get<NetworkInterface::SUBNET_MASK>();
			if (!addr.isWildcard()) std::cout << "Subnet:      " << addr.toString() << " (/" << addr.prefixLength() << ")" << std::endl;
			addr = ipIt->get<NetworkInterface::BROADCAST_ADDRESS>();
			if (!addr.isWildcard()) std::cout << "Broadcast:   " << addr.toString() << std::endl;
		}

		std::cout << "=============" << std::endl << std::endl;
	}
}


void NetworkInterfaceTest::testList()
{
	NetworkInterface::List list = NetworkInterface::list();
	assert (!list.empty());
	for (NetworkInterface::NetworkInterfaceList::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		std::cout << "==============" << std::endl;

		std::cout << "Index:       " << it->index() << std::endl;
		std::cout << "Name:        " << it->name() << std::endl;
		std::cout << "DisplayName: " << it->displayName() << std::endl;

		typedef NetworkInterface::AddressList List;
		const List& ipList = it->addressList();
		List::const_iterator ipIt = ipList.begin();
		List::const_iterator ipEnd = ipList.end();
		for (int counter = 0; ipIt != ipEnd; ++ipIt, ++counter)
		{
			std::cout << "IP Address:  " << ipIt->get<NetworkInterface::IP_ADDRESS>().toString() << std::endl;
			IPAddress addr = ipIt->get<NetworkInterface::SUBNET_MASK>();
			if (!addr.isWildcard()) std::cout << "Subnet:      " << ipIt->get<NetworkInterface::SUBNET_MASK>().toString() << std::endl;
			addr = ipIt->get<NetworkInterface::BROADCAST_ADDRESS>();
			if (!addr.isWildcard()) std::cout << "Broadcast:   " << ipIt->get<NetworkInterface::BROADCAST_ADDRESS>().toString() << std::endl;
		}

		std::cout << "==============" << std::endl << std::endl;
	}
}


void NetworkInterfaceTest::testForName()
{
	NetworkInterface::Map map = NetworkInterface::map();
	for (NetworkInterface::Map::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		NetworkInterface ifc = NetworkInterface::forName(it->second.name());
		assert (ifc.name() == it->second.name());
	}
}


void NetworkInterfaceTest::testForAddress()
{
	NetworkInterface::Map map = NetworkInterface::map();
	for (NetworkInterface::Map::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		// not all interfaces have IP configured
		if (it->second.addressList().empty()) continue;

		if (it->second.supportsIPv4())
		{
			NetworkInterface ifc = NetworkInterface::forAddress(it->second.findFirstAddress(IPAddress::IPv4));
			assert (ifc.findFirstAddress(IPAddress::IPv4) == it->second.findFirstAddress(IPAddress::IPv4));
		}
		else
		{
			try
			{
				it->second.findFirstAddress(IPAddress::IPv4);
				fail ("must throw");
			}
			catch (NotFoundException&) { }
		}
	}
}


void NetworkInterfaceTest::testForIndex()
{
	NetworkInterface::Map map = NetworkInterface::map();
	for (NetworkInterface::Map::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		NetworkInterface ifc = NetworkInterface::forIndex(it->second.index());
		assert (ifc.index() == it->second.index());
	}
}


void NetworkInterfaceTest::setUp()
{
}


void NetworkInterfaceTest::tearDown()
{
}


CppUnit::Test* NetworkInterfaceTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("NetworkInterfaceTest");

	CppUnit_addTest(pSuite, NetworkInterfaceTest, testList);
	CppUnit_addTest(pSuite, NetworkInterfaceTest, testMap);
	CppUnit_addTest(pSuite, NetworkInterfaceTest, testForName);
	CppUnit_addTest(pSuite, NetworkInterfaceTest, testForAddress);
	CppUnit_addTest(pSuite, NetworkInterfaceTest, testForIndex);

	return pSuite;
}
