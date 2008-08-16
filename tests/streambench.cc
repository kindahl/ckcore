#include <iostream>
#include "../src/types.hh"
#include "../src/file.hh"
#include "../src/filestream.hh"
#include "../src/bufferedstream.hh"
#include "../src/system.hh"

int main(int argc,const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: streambench <file read tests>" << std::endl;
        return 1;
    }

    if (!ckcore::File::Exist(argv[1]))
    {
        std::cerr << "Error: The specified file does not exist." << std::endl;
        return 1;
    }

    ckcore::FileInStream fs(argv[1]);
    if (!fs.Open())
    {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return 1;
    }

    ckcore::BufferedInStream is(fs);

    ckcore::tuint64 last_time = ckcore::System::Time();
    ckcore::tuint64 last_read = 0;

    unsigned char buffer[100];
    while (!is.End())
    {
        ckcore::tint64 res = is.Read(buffer,sizeof(buffer));
        if (res == -1)
        {
            std::cerr << "Error: Reading failed." << std::endl;
            return 1;
        }

        last_read += res;

        // Check if a second has passed.
        ckcore::tuint64 cur_time = ckcore::System::Time();
        if (cur_time - last_time > 1000)
        {
            std::cout << "Speed: " << last_read/(1024*1024) << " MiB/s." << std::endl;
            last_time = cur_time;
            last_read = 0;
        }
    }

    return 0;
}

