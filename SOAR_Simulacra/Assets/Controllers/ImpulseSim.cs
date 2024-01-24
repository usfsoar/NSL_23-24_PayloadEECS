using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ImpulseSim : MonoBehaviour
{
    public List<GameObject> propellers;
    public float baseMaxThrust = 1000f;
    public float maxThrust;
    public float lateralThrustMultiplier = 0.5f; // Adjust this value for lateral movement
    public float thrustChangeSpeed = 100f; // Speed at which max thrust changes
    List<Rigidbody> propRigidbodies;

    // Start is called before the first frame update
    void Start()
    {
        // Get the rigidbodies of the propellers
        propRigidbodies = new List<Rigidbody>();
        foreach (GameObject prop in propellers)
        {
            try { propRigidbodies.Add(prop.GetComponent<Rigidbody>()); }
            catch { Debug.Log("No rigidbody found on propeller"); }
        }
        maxThrust = baseMaxThrust;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        FlightController();
        AdjustMaxThrust();
    }

    void FlightController()
    {
        float forwardThrust = 1.0f;
        float backwardThrust = 1.0f;
        float lateralThrust = 0.0f;

        // Check for user input
        if (Input.GetKey(KeyCode.UpArrow))
        {
            forwardThrust = 1.1f;
        }
        else if (Input.GetKey(KeyCode.DownArrow))
        {
            backwardThrust = 1.1f;
        }

        if (Input.GetKey(KeyCode.LeftArrow))
        {
            lateralThrust = -lateralThrustMultiplier;
        }
        else if (Input.GetKey(KeyCode.RightArrow))
        {
            lateralThrust = lateralThrustMultiplier;
        }

        // Add thrust to each propeller
        for (int i = 0; i < propRigidbodies.Count; i++)
        {
            float thrustMultiplier = i < 2 ? forwardThrust : backwardThrust;
            AddThrust(propRigidbodies[i], thrustMultiplier, lateralThrust);
        }
    }

    void AddThrust(Rigidbody prop, float percent_thrust, float lateral_thrust)
    {
        var thrust = maxThrust * percent_thrust;
        var lateralForce = maxThrust * lateral_thrust;

        // Add thrust to the negative y-axis of the propeller
        prop.AddRelativeForce(new Vector3(0, thrust, 0));
        // Add lateral thrust to the x-axis of the propeller
        prop.AddRelativeForce(new Vector3(lateralForce, 0, 0));
        
        // Shoot debug rays to show the direction of the thrust
        Debug.DrawRay(prop.transform.position, prop.transform.up * thrust, Color.red);
        Debug.DrawRay(prop.transform.position, prop.transform.right * lateralForce, Color.blue);
    }

    void AdjustMaxThrust()
    {
        // Increase max thrust if W key is pressed
        if (Input.GetKey(KeyCode.W))
        {
            maxThrust += thrustChangeSpeed * Time.deltaTime;
        }
        // Decrease max thrust if S key is pressed
        else if (Input.GetKey(KeyCode.S))
        {
            maxThrust -= thrustChangeSpeed * Time.deltaTime;
            maxThrust = Mathf.Max(maxThrust, 0); // Ensure max thrust doesn't go below 0
        }
    }
}
