using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Data;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data.SqlClient;
using System.Net.Mail;
using System.Configuration;

public partial class api_Login : WOApiWebPage
{
    void AddUseToItem()
    {
        string skey1 = web.Param("skey1");
        if (skey1 != SERVER_API_KEY)
            throw new ApiExitException("bad key");

        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_Exchange_Add";
        sqcmd.Parameters.AddWithValue("@in_ItemID", web.Param("ItemID"));

        if (!CallWOApi(sqcmd))
            return;

        reader.Read();
        int ExchangeResultCode = getInt("ExchangeResultCode");
        int ShowInHUD = getInt("ShowInHUD");

        GResponse.Write("WO_0");
        GResponse.Write(string.Format("{0} {1}", ExchangeResultCode, ShowInHUD));
    }

    void CheckUseForItem()
    {
        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_Exchange_Check";
        sqcmd.Parameters.AddWithValue("@in_ItemID", web.Param("ItemID"));

        if (!CallWOApi(sqcmd))
            return;

        reader.Read();
        int UseCount = getInt("UseCount");
        int MaxUse = getInt("MaxUse");

        GResponse.Write("WO_0");
        GResponse.Write(string.Format("{0} {1}", UseCount, MaxUse));
    }

    void ReadUseForItem()
    {
        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_Exchange_Read";

        if (!CallWOApi(sqcmd))
            return;

        StringBuilder xml = new StringBuilder();
        xml.Append("<?xml version=\"1.0\"?>\n");
        xml.Append("<Exchange>\n");
        while (reader.Read())
        {
            int ItemID = Convert.ToInt32(reader["ItemID"]);
            int UseCount = Convert.ToInt32(reader["UseCount"]);
            int MaxUse = Convert.ToInt32(reader["MaxUse"]);

            xml.Append("<Item ");
            xml.Append(xml_attr("itemID", ItemID));
            xml.Append(xml_attr("UseCount", UseCount));
            xml.Append(xml_attr("MaxUse", MaxUse));
            xml.Append("/>");
        }
        xml.Append("</Exchange>\n");
        GResponse.Write(xml.ToString());
    }

    protected override void Execute()
    {
        if (!WoCheckLoginSession())
            return;

        string func = web.Param("func");

        if (func == "check")
            CheckUseForItem();
        else if (func == "add")
            AddUseToItem();
        else if (func == "read")
            ReadUseForItem();
        else
            throw new ApiExitException("bad func");
    }
}
