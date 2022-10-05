#include <iostream>
#include <clickhouse/client.h>


using namespace clickhouse;

int main() {

    // 初始化客戶端連線
    Client client(clickhouse::ClientOptions().SetHost("localhost").SetUser("default").SetPassword("password").SetDefaultDatabase("default"));

    // 刪除資料表
    client.Execute("DROP TABLE IF EXISTS default.numbers");

    // 建立資料表
    client.Execute("CREATE TABLE IF NOT EXISTS default.numbers (id UInt64, name String) ENGINE = Memory");

    // 寫入資料到上述的資料表
    {
        Block block;

        auto id = std::make_shared<ColumnUInt64>();
        id->Append(1);
        id->Append(7);

        auto name = std::make_shared<ColumnString>();
        name->Append("one");
        name->Append("seven");

        block.AppendColumn("id"  , id);
        block.AppendColumn("name", name);

        client.Insert("default.numbers", block);
    }

    // 篩選上一步寫入資料表的資料
    client.Select("SELECT id, name FROM default.numbers", [] (const Block& block)
        {
            for (size_t i = 0; i < block.GetRowCount(); ++i) {
                std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
                          << block[1]->As<ColumnString>()->At(i) << "\n";
            }
        }
    );

    // 刪除資料表
    client.Execute("DROP TABLE default.numbers");

    return 0;
}
