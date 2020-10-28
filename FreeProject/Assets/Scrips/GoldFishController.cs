using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoldFishController : MonoBehaviour
{
    Rigidbody GoldFish;

    public float speed = 0.1f;
    private float intervalTime;
    private float rotationTime;
    private float rotationRandomAngle;

    private bool flag;
    public double moterflag;

    private void Start()
    {
        GoldFish = this.GetComponent<Rigidbody>();

        intervalTime = 0;
        rotationTime = Random.Range(1.0f, 3.0f);
        flag = true;
        moterflag = 0;
    }

    private void Update()
    {
        if (flag == true)
        {
            transform.Translate(Vector3.right * speed);
        }

        intervalTime += Time.deltaTime;

        
        if (intervalTime >= rotationTime)
        {
            intervalTime = 0.0f;
            rotationTime = Random.Range(3.0f, 5.0f);

            transform.eulerAngles = new Vector3(0, Random.Range(-180f, 180f), 0);
        }
        //Debug.Log(GoldFish.velocity.y);
    }

    void OnCollisionEnter(Collision col)
    {
        intervalTime = 0;

        if (col.gameObject.name == "WallRight")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, 180f - angleDir, 0);
        }
        if (col.gameObject.name == "WallLeft")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, 180f - angleDir, 0);
        }
        if (col.gameObject.name == "WallBack")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, 360 - angleDir, 0);
        }
        if (col.gameObject.name == "WallFront")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, -angleDir, 0);
        }

        if (col.gameObject.name == "Bowl_right")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, 180f - angleDir, 0);
        }
        if (col.gameObject.name == "Bowl_front")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, -angleDir, 0);
        }
        if (col.gameObject.name == "Bowl_back")
        {
            float angleDir = transform.eulerAngles.y;
            //Debug.Log(angleDir);
            transform.eulerAngles = new Vector3(0, 360 - angleDir, 0);
        }
    }

    private void OnCollisionStay(Collision collision)
    {
        if(collision.gameObject.name == "Paper")
        { 
            flag = false;
            moterflag = 1;
        }
    }
    private void OnCollisionExit(Collision collision)
    {
        if(collision.gameObject.name == "Paper")
        {
            flag = true;
            moterflag = 0;
            Stop();
            //GoldFish.AddForce(0, 9.8f, 0);
        }

        //Debug.Log(GoldFish.velocity.y);
    }

    void Stop()
    {
        GoldFish.velocity = Vector3.zero;
        GoldFish.angularVelocity = Vector3.zero;
    }
}
