using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Data;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data.SqlClient;
using System.Configuration;

public partial class api_AccApplyKey : WOApiWebPage
{
    bool CheckCharPin(string pinkey)
    {
        char[] AllowedChars = " 1234567890".ToCharArray();
        foreach (char c in pinkey)
        {
            if (c.ToString().ToLower().IndexOfAny(AllowedChars) == -1)
            {
                return false;
            }
        }
        return true;
    }

    void SetPin()
    {
        string Pin = web.Param("serial");
        if (!CheckCharPin(Pin))
        {
            Response.Write("WO_7");
            Response.Write("Character pin cannot contain special symbols");
            return;
        }

        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_ACCOUNT_PIN_SETKEY";
        sqcmd.Parameters.AddWithValue("@in_CustomerID", web.CustomerID());
        sqcmd.Parameters.AddWithValue("@in_SerialKey", web.Param("serial"));

        if (!CallWOApi(sqcmd))
            return;

        Response.Write("WO_0");
    }

    void UnlockPin()
    {
        string Pin = web.Param("serial");
        if (!CheckCharPin(Pin))
        {
            Response.Write("WO_7");
            Response.Write("Character pin cannot contain special symbols");
            return;
        }

        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_ACCOUNT_PIN_UNLOCK";
        sqcmd.Parameters.AddWithValue("@in_CustomerID", web.CustomerID());
        sqcmd.Parameters.AddWithValue("@in_SerialKey", web.Param("serial"));

        if (!CallWOApi(sqcmd))
            return;

        Response.Write("WO_0");
    }

    void ResetFiestLoginPin()
    {
        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_ACCOUNT_PIN_RESET";
        sqcmd.Parameters.AddWithValue("@in_CustomerID", web.CustomerID());

        if (!CallWOApi(sqcmd))
            return;

        Response.Write("WO_0");
    }

    void UpgradeAccount()
    {
        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandType = CommandType.StoredProcedure;
        sqcmd.CommandText = "WZ_ACCOUNT_APPLYKEY";
        sqcmd.Parameters.AddWithValue("@in_CustomerID", web.CustomerID());
        sqcmd.Parameters.AddWithValue("@in_SerialKey", web.Param("serial"));

        if (!CallWOApi(sqcmd))
            return;

        Response.Write("WO_0");
    }

    protected override void Execute()
    {
        if (!WoCheckLoginSession())
            return;

        string func = web.Param("func");
        if (func == "upg")// upgrade account
            UpgradeAccount();
        else if (func == "pin")
            SetPin();
        else if (func == "unp")
            UnlockPin();
        else if (func == "res")
            ResetFiestLoginPin();
        else
            throw new ApiExitException("bad func");
    }
}