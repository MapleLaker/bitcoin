// Copyright (c) 2012-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <dbwrapper.h>
#include <uint256.h>
#include <random.h>
#include <test/test_bitcoin.h>

#include <memory>

#include <boost/test/unit_test.hpp>

// Test if a string consists entirely of null characters
static bool is_null_key(const std::vector<unsigned char>& key) {
    bool isnull = true;

    for (unsigned int i = 0; i < key.size(); i++)
        isnull &= (key[i] == '\x00');

    return isnull;
}
 
BOOST_FIXTURE_TEST_SUITE(dbwrapper_tests, BasicTestingSetup)
                       
BOOST_AUTO_TEST_CASE(dbwrapper)
{
    // Perform tests both obfuscated and non-obfuscated.
    for (bool obfuscate : {false, true}) {
        fs::path ph = SetDataDir(std::string("dbwrapper").append(obfuscate ? "_true" : "_false"));
        CDBWrapper dbw(ph, (1 << 20), true, false, obfuscate);
        char key = 'k';
        uint256 in = InsecureRand256();
        uint256 res;

        // Ensure that we're doing real obfuscation when obfuscate=true
        BOOST_CHECK(obfuscate != is_null_key(dbwrapper_private::GetObfuscateKey(dbw)));

        BOOST_CHECK(dbw.Write(key, in));
        BOOST_CHECK(dbw.Read(key, res));
        BOOST_CHECK_EQUAL(res.ToString(), in.ToString());
    }
}

BOOST_AUTO_TEST_CASE(dbwrapper_basic_data)
{
    // Perform tests both obfuscated and non-obfuscated.
    for (bool obfuscate : {false, true}) {
        fs::path ph = SetDataDir(std::string("dbwrapper_1").append(obfuscate ? "_true" : "_false"));
        //CDBWrapper dbw(ph, (1 << 20), true, false, obfuscate);
        CDBWrapper dbw(ph, (1 << 20), false, true, obfuscate);

        uint256 res;
        uint32_t res_uint_32;
        bool res_bool;

        // Ensure that we're doing real obfuscation when obfuscate=true
        BOOST_CHECK(obfuscate != is_null_key(dbwrapper_private::GetObfuscateKey(dbw)));
 
        //Simulate block raw data - "b + block hash"
        std::string block_tag = "b";
        uint256 block_hash = InsecureRand256();
        std::string key_block = block_tag + block_hash.ToString();

        uint256 in_block = InsecureRand256();
        BOOST_CHECK(dbw.Write(key_block, in_block));
        BOOST_CHECK(dbw.Read(key_block, res));
        BOOST_CHECK_EQUAL(res.ToString(), in_block.ToString());    

        //Simulate file raw data - "f + file_number"
        std::string file_tag = "f";
        uint32_t file_number = InsecureRand32();
        std::string key_file = strprintf("%s%04x", file_tag,file_number);

        uint256 in_file_info = InsecureRand256();
        BOOST_CHECK(dbw.Write(key_file, in_file_info));
        BOOST_CHECK(dbw.Read(key_file, res));
        BOOST_CHECK_EQUAL(res.ToString(), in_file_info.ToString());

        //Simulate transaction raw data - "t + transaction hash"
        std::string transaction_tag = "t";
        uint256 transaction_hash = InsecureRand256();
        std::string key_transaction = transaction_tag + transaction_hash.ToString();

        uint256 in_transaction = InsecureRand256();
        BOOST_CHECK(dbw.Write(key_transaction, in_transaction));
        BOOST_CHECK(dbw.Read(key_transaction, res));
        BOOST_CHECK_EQUAL(res.ToString(), in_transaction.ToString());      

        //Simulate UTXO raw data - "c + transaction hash"
        std::string utxo_tag = "c";
        uint256 utxo_hash = InsecureRand256();
        std::string key_utxo = utxo_tag + utxo_hash.ToString();

        uint256 in_utxo = InsecureRand256();
        BOOST_CHECK(dbw.Write(key_utxo, in_utxo));
        BOOST_CHECK(dbw.Read(key_utxo, res));
        BOOST_CHECK_EQUAL(res.ToString(), in_utxo.ToString()); 

        //Simulate last block file number - "l"
        char key_last_blockfile_number = 'l';
        uint32_t lastblockfilenumber = InsecureRand32();
        BOOST_CHECK(dbw.Write(key_last_blockfile_number, lastblockfilenumber));
        BOOST_CHECK(dbw.Read(key_last_blockfile_number, res_uint_32));
        BOOST_CHECK_EQUAL(lastblockfilenumber, res_uint_32);  

        //Simulate Is Reindexing - "R"
        char key_IsReindexing = 'R';
        bool isInReindexing = InsecureRandBool();
        BOOST_CHECK(dbw.Write(key_IsReindexing, isInReindexing));
        BOOST_CHECK(dbw.Read(key_IsReindexing, res_bool));
        BOOST_CHECK_EQUAL(isInReindexing, res_bool);   

        //Simulate last block hash up to which UXTO covers - 'B'
        char key_lastblockhash_uxto = 'B';
        uint256 lastblock_hash = InsecureRand256();
        BOOST_CHECK(dbw.Write(key_lastblockhash_uxto, lastblock_hash));
        BOOST_CHECK(dbw.Read(key_lastblockhash_uxto, res));
        BOOST_CHECK_EQUAL(lastblock_hash, res); 

        //Simulate file raw data - "F + filename_number + filename"
        std::string file_option_tag = "F";
        uint8_t filename_length = InsecureRandBits(8);
        std::string filename = "randomfilename";
        std::string key_file_option = strprintf("%s%01x%s", file_option_tag,filename_length,filename);

        bool in_file_bool = InsecureRandBool();
        BOOST_CHECK(dbw.Write(key_file_option, in_file_bool));
        BOOST_CHECK(dbw.Read(key_file_option, res_bool));
        BOOST_CHECK_EQUAL(res_bool, in_file_bool);                      
   }
}

// Test batch operations
BOOST_AUTO_TEST_CASE(dbwrapper_batch)
{
    // Perform tests both obfuscated and non-obfuscated.
    for (bool obfuscate : {false, true}) {
        fs::path ph = SetDataDir(std::string("dbwrapper_batch").append(obfuscate ? "_true" : "_false"));
        CDBWrapper dbw(ph, (1 << 20), true, false, obfuscate);

        char key = 'i';
        uint256 in = InsecureRand256();
        char key2 = 'j';
        uint256 in2 = InsecureRand256();
        char key3 = 'k';
        uint256 in3 = InsecureRand256();

        uint256 res;
        CDBBatch batch(dbw);

        batch.Write(key, in);
        batch.Write(key2, in2);
        batch.Write(key3, in3);

        // Remove key3 before it's even been written
        batch.Erase(key3);

        dbw.WriteBatch(batch);

        BOOST_CHECK(dbw.Read(key, res));
        BOOST_CHECK_EQUAL(res.ToString(), in.ToString());
        BOOST_CHECK(dbw.Read(key2, res));
        BOOST_CHECK_EQUAL(res.ToString(), in2.ToString());

        // key3 should've never been written
        BOOST_CHECK(dbw.Read(key3, res) == false);
    }
}

BOOST_AUTO_TEST_CASE(dbwrapper_iterator)
{
    // Perform tests both obfuscated and non-obfuscated.
    for (bool obfuscate : {false, true}) {
        fs::path ph = SetDataDir(std::string("dbwrapper_iterator").append(obfuscate ? "_true" : "_false"));
        CDBWrapper dbw(ph, (1 << 20), true, false, obfuscate);

        // The two keys are intentionally chosen for ordering
        char key = 'j';
        uint256 in = InsecureRand256();
        BOOST_CHECK(dbw.Write(key, in));
        char key2 = 'k';
        uint256 in2 = InsecureRand256();
        BOOST_CHECK(dbw.Write(key2, in2));

        std::unique_ptr<CDBIterator> it(const_cast<CDBWrapper&>(dbw).NewIterator());

        // Be sure to seek past the obfuscation key (if it exists)
        it->Seek(key);

        char key_res;
        uint256 val_res;

        it->GetKey(key_res);
        it->GetValue(val_res);
        BOOST_CHECK_EQUAL(key_res, key);
        BOOST_CHECK_EQUAL(val_res.ToString(), in.ToString());

        it->Next();

        it->GetKey(key_res);
        it->GetValue(val_res);
        BOOST_CHECK_EQUAL(key_res, key2);
        BOOST_CHECK_EQUAL(val_res.ToString(), in2.ToString());

        it->Next();
        BOOST_CHECK_EQUAL(it->Valid(), false);
    }
}

// Test that we do not obfuscation if there is existing data.
BOOST_AUTO_TEST_CASE(existing_data_no_obfuscate)
{
    // We're going to share this fs::path between two wrappers
    fs::path ph = SetDataDir("existing_data_no_obfuscate");
    create_directories(ph);

    // Set up a non-obfuscated wrapper to write some initial data.
    std::unique_ptr<CDBWrapper> dbw = MakeUnique<CDBWrapper>(ph, (1 << 10), false, false, false);
    char key = 'k';
    uint256 in = InsecureRand256();
    uint256 res;

    BOOST_CHECK(dbw->Write(key, in));
    BOOST_CHECK(dbw->Read(key, res));
    BOOST_CHECK_EQUAL(res.ToString(), in.ToString());

    // Call the destructor to free leveldb LOCK
    dbw.reset();

    // Now, set up another wrapper that wants to obfuscate the same directory
    CDBWrapper odbw(ph, (1 << 10), false, false, true);

    // Check that the key/val we wrote with unobfuscated wrapper exists and 
    // is readable.
    uint256 res2;
    BOOST_CHECK(odbw.Read(key, res2));
    BOOST_CHECK_EQUAL(res2.ToString(), in.ToString());

    BOOST_CHECK(!odbw.IsEmpty()); // There should be existing data
    BOOST_CHECK(is_null_key(dbwrapper_private::GetObfuscateKey(odbw))); // The key should be an empty string

    uint256 in2 = InsecureRand256();
    uint256 res3;
 
    // Check that we can write successfully
    BOOST_CHECK(odbw.Write(key, in2));
    BOOST_CHECK(odbw.Read(key, res3));
    BOOST_CHECK_EQUAL(res3.ToString(), in2.ToString());
}
                        
// Ensure that we start obfuscating during a reindex.
BOOST_AUTO_TEST_CASE(existing_data_reindex)
{
    // We're going to share this fs::path between two wrappers
    fs::path ph = SetDataDir("existing_data_reindex");
    create_directories(ph);

    // Set up a non-obfuscated wrapper to write some initial data.
    std::unique_ptr<CDBWrapper> dbw = MakeUnique<CDBWrapper>(ph, (1 << 10), false, false, false);
    char key = 'k';
    uint256 in = InsecureRand256();
    uint256 res;

    BOOST_CHECK(dbw->Write(key, in));
    BOOST_CHECK(dbw->Read(key, res));
    BOOST_CHECK_EQUAL(res.ToString(), in.ToString());

    // Call the destructor to free leveldb LOCK
    dbw.reset();

    // Simulate a -reindex by wiping the existing data store
    CDBWrapper odbw(ph, (1 << 10), false, true, true);

    // Check that the key/val we wrote with unobfuscated wrapper doesn't exist
    uint256 res2;
    BOOST_CHECK(!odbw.Read(key, res2));
    BOOST_CHECK(!is_null_key(dbwrapper_private::GetObfuscateKey(odbw)));

    uint256 in2 = InsecureRand256();
    uint256 res3;
 
    // Check that we can write successfully
    BOOST_CHECK(odbw.Write(key, in2));
    BOOST_CHECK(odbw.Read(key, res3));
    BOOST_CHECK_EQUAL(res3.ToString(), in2.ToString());
}

BOOST_AUTO_TEST_CASE(iterator_ordering)
{
    fs::path ph = SetDataDir("iterator_ordering");
    CDBWrapper dbw(ph, (1 << 20), true, false, false);
    for (int x=0x00; x<256; ++x) {
        uint8_t key = x;
        uint32_t value = x*x;
        if (!(x & 1)) BOOST_CHECK(dbw.Write(key, value));
    }

    // Check that creating an iterator creates a snapshot
    std::unique_ptr<CDBIterator> it(const_cast<CDBWrapper&>(dbw).NewIterator());

    for (unsigned int x=0x00; x<256; ++x) {
        uint8_t key = x;
        uint32_t value = x*x;
        if (x & 1) BOOST_CHECK(dbw.Write(key, value));
    }

    for (int seek_start : {0x00, 0x80}) {
        it->Seek((uint8_t)seek_start);
        for (unsigned int x=seek_start; x<255; ++x) {
            uint8_t key;
            uint32_t value;
            BOOST_CHECK(it->Valid());
            if (!it->Valid()) // Avoid spurious errors about invalid iterator's key and value in case of failure
                break;
            BOOST_CHECK(it->GetKey(key));
            if (x & 1) {
                BOOST_CHECK_EQUAL(key, x + 1);
                continue;
            }
            BOOST_CHECK(it->GetValue(value));
            BOOST_CHECK_EQUAL(key, x);
            BOOST_CHECK_EQUAL(value, x*x);
            it->Next();
        }
        BOOST_CHECK(!it->Valid());
    }
}

struct StringContentsSerializer {
    // Used to make two serialized objects the same while letting them have different lengths
    // This is a terrible idea
    std::string str;
    StringContentsSerializer() {}
    explicit StringContentsSerializer(const std::string& inp) : str(inp) {}

    StringContentsSerializer& operator+=(const std::string& s) {
        str += s;
        return *this;
    }
    StringContentsSerializer& operator+=(const StringContentsSerializer& s) { return *this += s.str; }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        if (ser_action.ForRead()) {
            str.clear();
            char c = 0;
            while (true) {
                try {
                    READWRITE(c);
                    str.push_back(c);
                } catch (const std::ios_base::failure& e) {
                    break;
                }
            }
        } else {
            for (size_t i = 0; i < str.size(); i++)
                READWRITE(str[i]);
        }
    }
};

BOOST_AUTO_TEST_CASE(iterator_string_ordering)
{
    char buf[10];

    fs::path ph = SetDataDir("iterator_string_ordering");
    CDBWrapper dbw(ph, (1 << 20), true, false, false);
    for (int x=0x00; x<10; ++x) {
        for (int y = 0; y < 10; y++) {
            snprintf(buf, sizeof(buf), "%d", x);
            StringContentsSerializer key(buf);
            for (int z = 0; z < y; z++)
                key += key;
            uint32_t value = x*x;
            BOOST_CHECK(dbw.Write(key, value));
        }
    }

    std::unique_ptr<CDBIterator> it(const_cast<CDBWrapper&>(dbw).NewIterator());
    for (int seek_start : {0, 5}) {
        snprintf(buf, sizeof(buf), "%d", seek_start);
        StringContentsSerializer seek_key(buf);
        it->Seek(seek_key);
        for (unsigned int x=seek_start; x<10; ++x) {
            for (int y = 0; y < 10; y++) {
                snprintf(buf, sizeof(buf), "%d", x);
                std::string exp_key(buf);
                for (int z = 0; z < y; z++)
                    exp_key += exp_key;
                StringContentsSerializer key;
                uint32_t value;
                BOOST_CHECK(it->Valid());
                if (!it->Valid()) // Avoid spurious errors about invalid iterator's key and value in case of failure
                    break;
                BOOST_CHECK(it->GetKey(key));
                BOOST_CHECK(it->GetValue(value));
                BOOST_CHECK_EQUAL(key.str, exp_key);
                BOOST_CHECK_EQUAL(value, x*x);
                it->Next();
            }
        }
        BOOST_CHECK(!it->Valid());
    }
}



BOOST_AUTO_TEST_SUITE_END()
