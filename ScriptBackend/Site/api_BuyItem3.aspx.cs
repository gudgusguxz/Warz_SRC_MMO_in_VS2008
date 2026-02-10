using System;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;

public partial class api_BuyItem3 : WOApiWebPage
{
    protected override void Execute()
    {
        // 1️⃣ check login/session
        if (!WoCheckLoginSession())
            return;

        // 2️⃣ Read parameters
        string buyIdx = web.Param("BuyIdx"); // ⚠️ ต้องเป็น string (ของเดิม)
        string itemIdStr = web.Param("ItemID");
        string qtyStr = web.Param("Qty");

        int customerId;
        int itemId;
        int qty;

        // 3️⃣ Validate + convert เฉพาะค่าที่ควรเป็น int
        if (!Int32.TryParse(web.CustomerID(), out customerId))
        {
            Response.Write("WO_5");
            return;
        }

        if (!Int32.TryParse(itemIdStr, out itemId))
        {
            Response.Write("WO_5");
            return;
        }

        if (!Int32.TryParse(qtyStr, out qty))
        {
            Response.Write("WO_5");
            return;
        }

        if (qty <= 0 || qty > 1000)
        {
            Response.Write("WO_5");
            return;
        }

        // 4️⃣ Prepare stored procedure (ซื้อ = 1 call)
        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;

        // ⚠️ ใช้ BuyIdx แบบ string ตาม API เดิม
        sqcmd.CommandText = BuyItem3.GetBuyProcFromIdx(buyIdx);

        // 5️⃣ Add parameters (กำหนด type ชัด = SQL เร็ว)
        sqcmd.Parameters.Add("@in_IP", SqlDbType.VarChar, 32).Value = LastIP;
        sqcmd.Parameters.Add("@in_CustomerID", SqlDbType.Int).Value = customerId;
        sqcmd.Parameters.Add("@in_ItemId", SqlDbType.Int).Value = itemId;
        sqcmd.Parameters.Add("@in_BuyDays", SqlDbType.Int)
            .Value = BuyItem3.GetBuyDaysFromIdx(buyIdx);
        sqcmd.Parameters.Add("@in_Qty", SqlDbType.Int).Value = qty;

        // 6️⃣ Call DB
        if (!CallWOApi(sqcmd))
            return;

        // 7️⃣ Read result
        if (!reader.Read())
        {
            Response.Write("WO_5");
            return;
        }

        int balance = getInt("Balance");
        long inventoryId = Convert.ToInt64(reader["InventoryID"]);

        // 8️⃣ Minimal response (เร็วสุด)
        Response.Write("WO_0 ");
        Response.Write(balance);
        Response.Write(" ");
        Response.Write(inventoryId);
    }
}
