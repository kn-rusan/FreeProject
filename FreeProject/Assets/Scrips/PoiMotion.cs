using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PoiMotion : MonoBehaviour
{
    public Rigidbody rb;
    GameObject gb;
    Client client;

    public float a = 0.9f;
    float poi_x;
    float poi_y;
    float poi_z;

    float poi_nowx;
    float poi_nowy;
    float poi_nowz;

    float poi_oldx;
    float poi_oldy;
    float poi_oldz;

    float vx;
    float vy;
    float vz;

    public float poi_speed = 8f;
    public float poi_ang = 0f;
    //private int count = 0;

    float[] theta = new float[Client.SenserNum];
    float[] sin = new float[Client.SenserNum];
    float[] cos = new float[Client.SenserNum];

    float[] init_theta = new float[Client.SenserNum];
    int init_count;

    void Start()
    {
        rb = GetComponent<Rigidbody>();
        gb = GameObject.Find("Client");
        client = gb.GetComponent<Client>();

        poi_oldx = rb.position.x;
        poi_oldy = rb.position.y;
        poi_oldz = rb.position.z;

        init_count = 0;

    }

    void Update()
    {
        if (init_count == 0)
        {
            for (int i = 0; i < init_theta.Length; i++)
            {
                init_theta[i] = client.num[i];
            }
            init_count = 1;
        }

        for (int i = 0; i < theta.Length; i++)
        {
            theta[i] = client.num[i];
            //Debug.Log(theta[i]);
        }

        theta[0] = 90f + 90 * (theta[0] - init_theta[0]) / 280;
        theta[1] = 180f + 90 * (theta[1] - init_theta[1]) / 25.5f;
        theta[2] = 90 * (theta[2] - init_theta[2]) * (-1) / 25.5f;

        /*
        theta[0] = 90f + 90 * (theta[0] - 408) / 280;
        theta[1] = 180f + 90 * (theta[1] - 200) / 25.5f;
        theta[2] = 90 * (theta[2] - 300) * (-1) / 25.5f;
        */
        //Debug.Log(theta[0]);
        //Debug.Log(theta[1]);
        //Debug.Log(theta[2]);
        //Debug.Log(theta[3]);

        for(int i = 0; i < sin.Length; i++)
        {
            sin[i] = Mathf.Sin(theta[i] * (Mathf.PI / 180));
            cos[i] = Mathf.Cos(theta[i] * (Mathf.PI / 180));
        }

        poi_nowx = -(30 * cos[0] + 30 * (sin[0] * sin[1] - cos[0] * cos[1])) * cos[2];
        poi_nowz = 30f - (35 * cos[0] + 35 * (sin[0] * sin[1] - cos[0] * cos[1])) * sin[2];
        poi_nowy = 30f - (30 * sin[0] - 30 * (sin[0] * cos[1] + cos[0] * sin[1]));
        //Debug.Log("x" + poi_x);
        //Debug.Log("y" + poi_y);

        //ローパスフィルタ
        poi_x = a * poi_oldx + (1 - a) * poi_nowx;
        poi_z = a * poi_oldz + (1 - a) * poi_nowz;
        poi_y = a * poi_oldy + (1 - a) * poi_nowy;

        //速度の計算
        vx = (poi_x - poi_oldx) / Time.deltaTime;
        vy = (poi_y - poi_oldy) / Time.deltaTime;
        vz = (poi_z - poi_oldz) / Time.deltaTime;

        rb.velocity = new Vector3(vx, vy, vz);

        /*
        Vector3 now_position = rb.position;
        now_position = new Vector3(poi_x, poi_y, poi_z);
        rb.position = now_position;
        */

        poi_oldx = poi_x;
        poi_oldy = poi_y;
        poi_oldz = poi_z;

        /*
        //キーボードによるポイの操作
        if(Input.GetKey(KeyCode.RightArrow))
        {
            //transform.Translate(Vector3.right * poi_speed * Time.deltaTime);
            rb.velocity = Vector3.right * poi_speed ;
        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            rb.velocity = Vector3.left * poi_speed;
        }
        if(Input.GetKey(KeyCode.UpArrow))
        {
            rb.velocity = Vector3.forward * poi_speed;
        }
        if (Input.GetKey(KeyCode.DownArrow))
        {
            rb.velocity = Vector3.back * poi_speed;
        }
        if(Input.GetKey(KeyCode.D))
        {
            rb.velocity = Vector3.down * poi_speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            rb.velocity = Vector3.up * poi_speed;
        }
        */


        //加速度センサによるポイの傾き操作
        Debug.Log(theta[3]);
        if(theta[3] >= 252)
        {
            poi_ang = 0;
        }else if(theta[3] < 252 && theta[3]>= 230)
        {
            poi_ang = 45;
        }else if(theta[3] < 230 && theta[3] >= 215)
        {
            poi_ang = 90;
        }else
        {
            poi_ang = 135;
        }

        rb.MoveRotation(Quaternion.AngleAxis(poi_ang, Vector3.forward));

        //Debug.Log(client.num);

        /*
        if(theta[3] == 1234)
        {
            if (Input.GetKeyDown(KeyCode.F))
            {
                rb.MoveRotation(Quaternion.AngleAxis(45f, Vector3.forward));
            }
            if (Input.GetKeyUp(KeyCode.F))
            {
                rb.MoveRotation(Quaternion.AngleAxis(0, Vector3.forward));
            }
        }
        else
        {
            if (count == 5)
            {
                count = 0;
                rb.MoveRotation(Quaternion.AngleAxis((260f - poi_ang / 5) * 3f, Vector3.forward));
                poi_ang = 0;
            }
            else
            {
                count++;
                poi_ang += client.num[3];
            }
        }
        */
        

        //rb.MoveRotation(Quaternion.AngleAxis((260f-client.num)*3f, Vector3.forward));

        /*
        if (Input.GetKeyUp(KeyCode.RightArrow) || Input.GetKeyUp(KeyCode.LeftArrow) || Input.GetKeyUp(KeyCode.UpArrow)
            || Input.GetKeyUp(KeyCode.DownArrow) || Input.GetKeyUp(KeyCode.D) || Input.GetKeyUp(KeyCode.S))
        {
            Stop();
        }
        */

    }

    void Stop()
    {
        rb.velocity = Vector3.zero;
        rb.angularVelocity = Vector3.zero;
    }

}
