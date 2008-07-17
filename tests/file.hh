#include <cxxtest/TestSuite.h>
#include <stdlib.h>
#include "../src/types.hh"
#include "../src/file.hh"

class FileTestSuite : public CxxTest::TestSuite
{
public:
    void testOpenCloseTest()
    {
        // Test non-existing file.
        ckCore::CFile File1("data/file/non-existent");
        TS_ASSERT(!File1.Test());

        TS_ASSERT(!File1.Open(ckCore::CFile::OPEN_READ));
        TS_ASSERT(!File1.Test());
        TS_ASSERT(!File1.Close());
        TS_ASSERT(!File1.Test());

        TS_ASSERT(File1.Open(ckCore::CFile::OPEN_WRITE));
        TS_ASSERT(File1.Test());
        TS_ASSERT(File1.Close());
        TS_ASSERT(!File1.Test());

        TS_ASSERT(File1.Delete());
        TS_ASSERT(!File1.Test());
        TS_ASSERT(!File1.Close());

        // Test existing file.
        ckCore::CFile File2("data/file/0bytes");
        TS_ASSERT(!File2.Test());

        TS_ASSERT(File2.Open(ckCore::CFile::OPEN_READ));
        TS_ASSERT(File2.Test());
        TS_ASSERT(File2.Close());
        TS_ASSERT(!File2.Test());

        TS_ASSERT(File2.Open(ckCore::CFile::OPEN_WRITE));
        TS_ASSERT(File2.Test());
        TS_ASSERT(File2.Close());
        TS_ASSERT(!File2.Test());
    }

    void testSeekTell()
    {
        ckCore::CFile File("data/file/8253bytes");
        TS_ASSERT(File.Open(ckCore::CFile::OPEN_READ));
        TS_ASSERT(File.Test());

        // Test exteme values.
        TS_ASSERT(File.Seek(0,ckCore::CFile::FILE_BEGIN) == 0);
        TS_ASSERT(File.Tell() == 0);
        TS_ASSERT(File.Seek(0,ckCore::CFile::FILE_CURRENT) == 0);
        TS_ASSERT(File.Tell() == 0);

        TS_ASSERT(File.Seek(0,ckCore::CFile::FILE_END) == File.Size());
        TS_ASSERT(File.Tell() == File.Size());

        int iFileSize = 8253;

        // Test random seeking.
        for (unsigned int i = 0; i < 100; i++)
        {
            TS_ASSERT(File.Seek(0,ckCore::CFile::FILE_BEGIN) == 0);

            int iSeek1 = rand() % (iFileSize >> 1);
            int iSeek2 = rand() % (iFileSize >> 1);
            int iSeekTot = iSeek1 + iSeek2;

            // Seek forward from the current position.
            TS_ASSERT(File.Seek(iSeek1,ckCore::CFile::FILE_CURRENT) == iSeek1);
            TS_ASSERT(File.Tell() == iSeek1);

            TS_ASSERT(File.Seek(iSeek2,ckCore::CFile::FILE_CURRENT) == iSeekTot);
            TS_ASSERT(File.Tell() == iSeekTot);

            // Seek backward from the current position.
            TS_ASSERT(File.Seek(-iSeek1,ckCore::CFile::FILE_CURRENT) == iSeek2);
            TS_ASSERT(File.Tell() == iSeek2);

            TS_ASSERT(File.Seek(-iSeek2,ckCore::CFile::FILE_CURRENT) == 0);
            TS_ASSERT(File.Tell() == 0);
        }
    }

    void testExistDelete()
    {
        ckCore::CFile File1("data/file/non-existent");
        TS_ASSERT(!File1.Exist());
        TS_ASSERT(!File1.Delete());
        TS_ASSERT(File1.Open(ckCore::CFile::OPEN_WRITE));
        TS_ASSERT(File1.Exist());
        TS_ASSERT(File1.Delete());
        TS_ASSERT(!File1.Exist());

        ckCore::CFile File2("data/file/0bytes");
        TS_ASSERT(File2.Exist());

        // Test static functions.
        TS_ASSERT(ckCore::CFile::Exist("data/file/0bytes"));
        TS_ASSERT(!ckCore::CFile::Exist("data/file/non-existent"));

        TS_ASSERT(!ckCore::CFile::Delete("data/file/non-existent"));
        ckCore::CFile File3("data/file/new");
        TS_ASSERT(File3.Open(ckCore::CFile::OPEN_WRITE));
        TS_ASSERT(File3.Close());
        TS_ASSERT(ckCore::CFile::Delete("data/file/new"));
        TS_ASSERT(!ckCore::CFile::Delete("data/file/non-existent"));
        TS_ASSERT(!ckCore::CFile::Delete(""));

    }

    void testRename()
    {
        // Rename file in existing folder (should succeed).
        ckCore::CFile File1("data/file/new");
        TS_ASSERT(File1.Open(ckCore::CFile::OPEN_WRITE));

        TS_ASSERT(File1.Rename("data/file/new2"));
        TS_ASSERT(ckCore::CFile::Exist("data/file/new2"));
        TS_ASSERT(!File1.Close());

        // Rename file in new non-existing folder (should fail).
        ckCore::CFile File2("data/file/new");
        TS_ASSERT(File2.Open(ckCore::CFile::OPEN_WRITE));

        TS_ASSERT(!File2.Rename("data/file/new/new2"));
        TS_ASSERT(!ckCore::CFile::Exist("data/file/new/new2"));
        TS_ASSERT(!File2.Close());

        // Try to rename file to existing file (should fail).
        TS_ASSERT(!File2.Rename("data/file/new2"));

        // Clean up.
        TS_ASSERT(File1.Delete());
        TS_ASSERT(File2.Delete());

        // Try to rename non-existent file.
        ckCore::CFile File3("data/file/non-existent");
        TS_ASSERT(!File3.Rename("data/file/new"));
        TS_ASSERT(!ckCore::CFile::Exist("data/file/new"));
        TS_ASSERT(!File3.Rename("data/file/non-existent"));
        TS_ASSERT(!ckCore::CFile::Exist("data/file/non-existent"));

        // Try to rename without name.
        TS_ASSERT(!File3.Rename(""));

        // Test static function.
        ckCore::CFile File4("data/file/new");
        TS_ASSERT(File4.Open(ckCore::CFile::OPEN_WRITE));
        TS_ASSERT(File4.Close());
        TS_ASSERT(ckCore::CFile::Rename("data/file/new","data/file/new2"));
        TS_ASSERT(!ckCore::CFile::Rename("data/file/new2","data/file/new2"));
        TS_ASSERT(!ckCore::CFile::Rename("data/file/new2",""));
        TS_ASSERT(!ckCore::CFile::Rename("data/file/new2","data/file/new/new2"));
        TS_ASSERT(ckCore::CFile::Delete("data/file/new2"));
    }

    void testSize()
    {
        const ckCore::TChar *szFilePaths[] =
        {
            "data/file/0bytes",
            "data/file/53bytes",
            "data/file/123bytes",
            "data/file/8253bytes"
        };

        const ckCore::TInt64 iFileSizes[] =
        {
            0,
            53,
            123,
            8253
        };

        for (unsigned int i = 0; i < 4; i++)
        {
            //TS_TRACE(szFilePaths[i]);
            ckCore::CFile File(szFilePaths[i]);
            TS_ASSERT_EQUALS(File.Size(),iFileSizes[i]);
            File.Open(ckCore::CFile::OPEN_READ);
            TS_ASSERT_EQUALS(File.Size(),iFileSizes[i]);
            File.Close();
            File.Open(ckCore::CFile::OPEN_WRITE);
            TS_ASSERT_EQUALS(File.Size(),iFileSizes[i]);
            File.Close();

            // Test static function.
            TS_ASSERT_EQUALS(ckCore::CFile::Size(szFilePaths[i]),iFileSizes[i]);
        }
    }
};

