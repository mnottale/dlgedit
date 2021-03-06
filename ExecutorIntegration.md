# Dlgedit dialog executor integration

The executor runs with your game code to execute the dialog json files.

Currently there is only one executor implementation, in C#, tested inside unity (but unity is not required).

It interacts with your game engine in three ways:

   - Display a text spoken by an NPC
   - Display a list of text to chose from for the PC and invoke a callback with the entry chosen
   - Evaluate a condition or an effect

Note that the engine is using futures for all asynchronous operations (no async currently).

## First step

Integrate the two source files Future.cs and Dialog.cs into your game code.
If you are using unity comment out the Debug static class. Otherwise leave it or edit it to connect it to your logging system.

## displaying text and PC choices

You must implement the two methods in DialogState class: PNJSays() and PJSays().

The first displays a text spoken by a NPC, and calls the given delegate when text has been validated by the player.

The second display a list of choices, and calls a callback with the chosen line index when validated by the player.

## Evaluating expressions

You must provide a static method "object Eval.eval(string text)" in charge of evaluation of the code generated by dlgedit's condition and effect buttons, and the code embeded in text by the "${}" syntax.

In condition boxes, the returned object will try to be interpreted as a boolean value using a leniant converter.

In effect boxes, you can return a Future, in which case it will be waited for by execution.

How you implement "eval()" is entirely up to you. We had great success in unity by using Jint, a Javascript engine bound C#, on which you can bind C# objects, potentially allowing full access to game code from dlgedit expressions.

But you can go an alternate route, such as writting your own DSL, or a simple parser.


For reference at the end of this document is a sample eval implementation using Jint and unity.

## Starting a dialog

Just calls Dialog.StartDialog(string pnjName, string json) giving as argument the main NPC name player is talking to, and the json content of the dialog file.



```c#

using UnityEngine;

using System.Collections;
using System.Collections.Generic;
using System;

using Jint;

public class Eval: MonoBehaviour
{
  static public string normalize(string n)
  {
    return n.Replace(" ", "").Replace("-", "_").Replace("(", "_").Replace(")", "_");
  }
  static public string BuildPrefix()
  {
    return "var u = importNamespace(\"UnityEngine\"); var GameObject = u.GameObject;";
  }
  static public void addContext(Engine eng, GameObject o)
  {
    eng.SetValue(normalize(o.name), (object)o);
    Component[] comps = o.GetComponents<Component>();
    foreach(Component c in comps)
    {
      if (c != null && c.GetType() != null)
        eng.SetValue(normalize(o.name) + c.GetType().Name, (object)c);
    }
    foreach (Transform t in o.transform)
    {
      addContext(eng, t.gameObject);
    }
  }
	static public void addContext(ParameterVariableHolder pvh, GameObject o)
	{
		pvh.Parameters[normalize(o.name)] = new Parameter(o);
    	Component[] comps = o.GetComponents<Component>();
    	foreach(Component c in comps)
    	{
    		pvh.Parameters[normalize(o.name) + c.GetType().Name] = new Parameter(c);
    	}
    	foreach (Transform t in o.transform)
    	{
    		addContext(pvh, t.gameObject);
    	}
	}

	static public object eval(string expr, Dictionary<string, object> ctx = null)
	{

	var engine = new Jint.Engine(cfg => cfg
        .AllowClr(typeof(GameObject).Assembly)
        );
    engine.SetValue("Helper", new Helper()); // we have a bunch of static methods in here
    engine.SetValue("quests", (object)GameObject.Find("state").GetComponent<Quest>());
    engine.SetValue("fight", (object)GameObject.Find("state").GetComponent<Fight>());
    var prefix = BuildPrefix();
    Eval.addContext(engine, GameObject.Find("maps"));
    Eval.addContext(engine, GameObject.Find("Main Camera"));
    Eval.addContext(engine, GameObject.Find("sounds"));
    Eval.addContext(engine, GameObject.Find("music"));
    if (ctx != null)
    {
    	foreach (var kv in ctx)
    	{
    		engine.SetValue(kv.Key, kv.Value);
    	}
    }
    return engine.Execute(prefix + expr).GetCompletionValue().ToObject();
	
	}
}
```