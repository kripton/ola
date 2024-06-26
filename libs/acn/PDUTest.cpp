/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * PDUTest.cpp
 * Test fixture for the PDU class
 * Copyright (C) 2005 Simon Newton
 */

#include <cppunit/extensions/HelperMacros.h>

#include "ola/Logging.h"
#include "ola/io/IOQueue.h"
#include "ola/io/OutputStream.h"
#include "ola/testing/TestUtils.h"
#include "libs/acn/PDU.h"
#include "libs/acn/PDUTestCommon.h"

namespace ola {
namespace acn {

using ola::io::IOQueue;
using ola::io::OutputStream;

class PDUTest: public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PDUTest);
  CPPUNIT_TEST(testPDU);
  CPPUNIT_TEST(testPDUBlock);
  CPPUNIT_TEST(testPDUToOutputStream);
  CPPUNIT_TEST(testBlockToOutputStream);
  CPPUNIT_TEST_SUITE_END();

 public:
    void testPDU();
    void testPDUBlock();
    void testPDUToOutputStream();
    void testBlockToOutputStream();

    void setUp() {
      ola::InitLogging(ola::OLA_LOG_DEBUG, ola::OLA_LOG_STDERR);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PDUTest);


/*
 * Test that packing a PDUBlock works.
 */
void PDUTest::testPDU() {
  MockPDU pdu(0x1234, 0x2468);

  OLA_ASSERT_EQ(4u, pdu.HeaderSize());
  OLA_ASSERT_EQ(4u, pdu.DataSize());
  OLA_ASSERT_EQ(14u, pdu.Size());

  unsigned int size = pdu.Size();
  uint8_t *data = new uint8_t[size];
  unsigned int bytes_used = size;
  OLA_ASSERT(pdu.Pack(data, &bytes_used));
  OLA_ASSERT_EQ(size, bytes_used);

  // test null data
  OLA_ASSERT_FALSE(pdu.Pack(NULL, &bytes_used));

  // test a null length
  OLA_ASSERT_FALSE(pdu.Pack(data, NULL));

  // test undersized buffer
  bytes_used = size - 1;
  OLA_ASSERT_FALSE(pdu.Pack(data, &bytes_used));
  OLA_ASSERT_EQ(0u, bytes_used);

  // test oversized buffer
  bytes_used = size + 1;
  OLA_ASSERT(pdu.Pack(data, &bytes_used));
  OLA_ASSERT_EQ(size, bytes_used);
  delete[] data;
}


/*
 * Test that packing a PDUBlock works.
 */
void PDUTest::testPDUBlock() {
  FakePDU pdu1(1);
  FakePDU pdu2(2);
  FakePDU pdu42(42);
  PDUBlock<FakePDU> block;
  block.AddPDU(&pdu1);
  block.AddPDU(&pdu2);
  block.AddPDU(&pdu42);

  unsigned int block_size = block.Size();
  OLA_ASSERT_EQ(12u, block_size);
  uint8_t *data = new uint8_t[block_size];
  unsigned int bytes_used = block_size;
  OLA_ASSERT(block.Pack(data, &bytes_used));
  OLA_ASSERT_EQ(block_size, bytes_used);

  unsigned int *test = (unsigned int*) data;
  OLA_ASSERT_EQ(1u, *test++);
  OLA_ASSERT_EQ(2u, *test++);
  OLA_ASSERT_EQ(42u, *test);

  // test null data
  OLA_ASSERT_FALSE(block.Pack(NULL, &bytes_used));

  // test a null length
  OLA_ASSERT_FALSE(block.Pack(data, NULL));

  // test undersized buffer
  bytes_used = block_size - 1;
  OLA_ASSERT_FALSE(block.Pack(data, &bytes_used));
  // TODO(Peter): Work out what behaviour we want for the bytes used, it's
  // currently the actual total used, not zero like the PDU::Pack returns
  // OLA_ASSERT_EQ(0u, bytes_used);

  // test oversized buffer
  bytes_used = block_size + 1;
  OLA_ASSERT(block.Pack(data, &bytes_used));
  OLA_ASSERT_EQ(block_size, bytes_used);
  delete[] data;

  block.Clear();
  OLA_ASSERT_EQ(0u, block.Size());
}


/*
 * Test that writing a PDU to an OutputStream works.
 */
void PDUTest::testPDUToOutputStream() {
  MockPDU pdu(0x1234, 0x2468);

  IOQueue output;
  OutputStream stream(&output);
  pdu.Write(&stream);
  OLA_ASSERT_EQ(14u, output.Size());

  uint8_t *data = new uint8_t[output.Size()];
  unsigned int data_size = output.Peek(data, output.Size());
  OLA_ASSERT_EQ(output.Size(), data_size);

  uint8_t EXPECTED[] = {
    0x70, 0x0e, 0, 0,
    0, 0x2b,
    0x34, 0x12, 0, 0,
    0x68, 0x24, 0, 0
  };
  OLA_ASSERT_DATA_EQUALS(EXPECTED, sizeof(EXPECTED), data, data_size);
  output.Pop(output.Size());

  // test null stream
  pdu.Write(NULL);
  delete[] data;
}


/*
 * Test that writing to an OutputStream works.
 */
void PDUTest::testBlockToOutputStream() {
  FakePDU pdu1(1);
  FakePDU pdu2(2);
  FakePDU pdu42(42);
  PDUBlock<FakePDU> block;
  block.AddPDU(&pdu1);
  block.AddPDU(&pdu2);
  block.AddPDU(&pdu42);

  IOQueue output;
  OutputStream stream(&output);
  block.Write(&stream);
  OLA_ASSERT_EQ(12u, output.Size());

  uint8_t *block_data = new uint8_t[output.Size()];
  unsigned int block_size = output.Peek(block_data, output.Size());
  OLA_ASSERT_EQ(output.Size(), block_size);

  uint8_t EXPECTED[] = {
    0, 0, 0, 1,
    0, 0, 0, 2,
    0, 0, 0, 42
  };
  OLA_ASSERT_DATA_EQUALS(EXPECTED, sizeof(EXPECTED), block_data, block_size);
  output.Pop(output.Size());

  // test null stream
  block.Write(NULL);
  delete[] block_data;
}
}  // namespace acn
}  // namespace ola
