using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Data;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data.SqlClient;

public partial class api_SrvAddLogTradeInfo : WOApiWebPage
{
    protected override void Execute()
    {
        if (!IsServerIP())
            throw new ApiExitException("bad key");

        string skey1 = web.Param("skey1");
        if (skey1 != SERVER_API_KEY)
            throw new ApiExitException("bad key");

        string IPStr = web.Param("IP");
        System.Net.IPAddress IpAddr = new System.Net.IPAddress(Convert.ToInt64(IPStr));

        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_SRV_AddLogTradeInfo";
        sqcmd.Parameters.AddWithValue("@in_CustomerID", web.CustomerID());
        sqcmd.Parameters.AddWithValue("@in_CharID", web.Param("CharID"));
        sqcmd.Parameters.AddWithValue("@in_Gamertag", web.Param("Gamertag"));
        sqcmd.Parameters.AddWithValue("@in_GameSessionID", web.Param("GameSessionID"));
        sqcmd.Parameters.AddWithValue("@in_CustomerIP", IpAddr.ToString());
        sqcmd.Parameters.AddWithValue("@in_Data", web.Param("Data"));
        sqcmd.Parameters.AddWithValue("@in_ItemID", web.Param("ItemID"));
        sqcmd.Parameters.AddWithValue("@in_Quantity", web.Param("Quantity"));
        sqcmd.Parameters.AddWithValue("@in_GameDollar", web.Param("GameDollar"));

        if (!CallWOApi(sqcmd))
            return;

        Response.Write("WO_0");
    }
}
