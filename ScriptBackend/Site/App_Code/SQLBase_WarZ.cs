using System;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;
using System.Diagnostics;

public class SQLBase : IDisposable
{
    private SqlConnection conn_;
    private static readonly string connStr =
        ConfigurationManager.ConnectionStrings["WarZ"].ConnectionString;

    public SQLBase()
    {
    }

    public bool Connect()
    {
        if (conn_ != null)
            return true;

        try
        {
            conn_ = new SqlConnection(connStr);
            conn_.Open();
            return true;
        }
        catch
        {
            throw new ApiExitException("SQL Connect failed");
        }
    }

    public void Disconnect()
    {
        if (conn_ != null)
        {
            try
            {
                conn_.Close();
                conn_.Dispose();
            }
            catch { }
            conn_ = null;
        }
    }

    public SqlDataReader Select(SqlCommand cmd)
    {
        if (conn_ == null)
            throw new ApiExitException("SQL not connected");

        try
        {
            cmd.Connection = conn_;
            cmd.CommandTimeout = 30;
            return cmd.ExecuteReader();
        }
        catch (SqlException ex)
        {
#if DEBUG
            DumpSqlCommand(cmd);
            throw new ApiExitException("SQL Select Failed: " + ex.Message);
#else
            throw new ApiExitException("ERR_SQL");
#endif
        }
    }

    private void DumpSqlCommand(SqlCommand cmd)
    {
        Debug.WriteLine("SQL CMD: " + cmd.CommandText);
        foreach (SqlParameter p in cmd.Parameters)
        {
            Debug.WriteLine(
                string.Format("{0}={1}", p.ParameterName, p.Value)
            );
        }
    }

    public void Dispose()
    {
        Disconnect();
    }
}
