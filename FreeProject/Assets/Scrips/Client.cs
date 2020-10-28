using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using UnityEngine;

public class Client : MonoBehaviour
{
    public static string data;
    public static string resMsg;

    public static int SenserCount = 0;
    public static int[] SenserVal = new int[1024];

    public static int SenserNum = 4;
    public int[] num = new int[SenserNum];

    System.Net.Sockets.NetworkStream ns;
    System.Net.Sockets.TcpClient tcp;

    GoldFishController goldfish0, goldfish1, goldfish2, goldfish3, goldfish4;

    // Use this for initialization
    void Start()
    {
        goldfish0 = GameObject.Find("Rantyu").GetComponent<GoldFishController>();
        goldfish1 = GameObject.Find("Rantyu (1)").GetComponent<GoldFishController>();
        goldfish2 = GameObject.Find("Rantyu (2)").GetComponent<GoldFishController>();
        goldfish3 = GameObject.Find("Rantyu (3)").GetComponent<GoldFishController>();
        goldfish4 = GameObject.Find("Rantyu (4)").GetComponent<GoldFishController>();

        //サーバに送信したいデータをぶち込む
        data = "1234567";
        //num = 1234;

        //サーバーのIPアドレスとポート番号
        string ipOrHost = "localhost";
        int port = 12345;

        //TcpClientを作成し、サーバーと接続
        tcp = new System.Net.Sockets.TcpClient(ipOrHost, port);
        Debug.Log("サーバー({0}:{1})と接続しました。" +
            ((System.Net.IPEndPoint)tcp.Client.RemoteEndPoint).Address + "," +
            ((System.Net.IPEndPoint)tcp.Client.RemoteEndPoint).Port + "," +
            ((System.Net.IPEndPoint)tcp.Client.LocalEndPoint).Address + "," +
            ((System.Net.IPEndPoint)tcp.Client.LocalEndPoint).Port);

        //NetworkStreamを取得
        ns = tcp.GetStream();
    }
    // Update is called once per frame
    void Update()
    {
        //↓にC++に送るデータをぶち込む
        //今回は経過時間
        if (goldfish0.moterflag == 1 || goldfish1.moterflag == 1 || goldfish2.moterflag == 1 || goldfish3.moterflag == 1 || goldfish4.moterflag == 1)
        {
            data = "1";
        }
        else data = "0";
        //Debug.Log(goldfish0.moterflag.ToString());
        //data = "1.0000";
        //data = Time.time.ToString();

        //タイムアウト設定
        ns.ReadTimeout = 10000;
        ns.WriteTimeout = 10000;

        //サーバーにデータを送信
        //文字列をByte型配列に変換
        System.Text.Encoding enc = System.Text.Encoding.UTF8;
        byte[] sendBytes = enc.GetBytes(data + '\n');
        //データを送信する
        ns.Write(sendBytes, 0, sendBytes.Length);
        //Debug.Log(data);


        //サーバーから送られたデータを受信する
        //今回は一周期分の時間が送られてくる。
        System.IO.MemoryStream ms = new System.IO.MemoryStream();
        byte[] resBytes = new byte[256];
        int resSize = 256;

        //データを受信
        resSize = ns.Read(resBytes, 0, resBytes.Length);
        //受信したデータを蓄積
        ms.Write(resBytes, 0, resSize);
        //受信したデータを文字列に変換
        resMsg = enc.GetString(ms.GetBuffer(), 0, (int)ms.Length);
        ms.Close();
        //末尾の\nを削除
        resMsg = resMsg.TrimEnd('\n');
        //Debug.Log(resMsg);

        //データをセンサの数だけ分割
        string[] str = resMsg.Split(' ');

        for (int i = 0; i < str.Length; i++)
        {
            //int型に変換
            num[i] = int.Parse(str[i]);
            //Debug.Log(num[i]);
        }


        //書き込みを行うセンサー値を取得
        /*
        SenserVal[SenserCount] = num[1];
        Debug.Log(SenserVal[SenserCount]);
        SenserCount += 1;
        */

        //rb.MoveRotation(Quaternion.AngleAxis(45f, Vector3.forward))Debug.Log(num);

        //スペース押すと閉じる
        if (Input.GetKey(KeyCode.Space))
        {
            //センサー値をCSV形式でエクスポート
            //logSave(SenserVal, "Test");

            ns.Close();
            tcp.Close();
            Debug.Log("切断しました。");
        }
    }

    //センサー値をCSV形式でエクスポートする関数
    public void logSave(int[] num, string filename)
    {
        StreamWriter sw;
        FileInfo fi;
        fi = new FileInfo(Application.dataPath + filename + ".csv");
        sw = fi.AppendText();

        for (int i = 0; i < num.Length; i++)
        {
            sw.WriteLine(num[i].ToString());
        }

        Console.WriteLine("File exported");
        sw.Flush();
        sw.Close();
    }
}