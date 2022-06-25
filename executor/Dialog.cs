using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json;

// Comment me if you are using unity
public static class Debug
{
  public static void Log(string msg)
  {
    Console.Log(msg);
  }
}

public class DlgSeqItemJson: JsonConverter
{
  public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
  {
    throw new Exception("not implemented");
  }
  
  public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
  {
    var res = new DlgSeqItem();
    if (reader.TokenType == JsonToken.String)
      res.text = (string)reader.Value;
    else
    { // startobject
      reader.Read(); // ->propertyname
      res.key = (string)reader.Value;
      reader.Read(); // ->propertyvalue
      res.value = (string)reader.Value;
      reader.Read(); // ->endobject
    }
    
    return res;
  }
  public override bool CanConvert(Type objectType)
  {
    return objectType == typeof(DlgSeqItem);
  }
}

[JsonConverter(typeof(DlgSeqItemJson))]
public class DlgSeqItem
{
  public string text;
  public string key;
  public string value;
}
public class DlgNode
{
  public string id;
  public List<DlgNode> list;
  public List<DlgNode> choices;
  public string condition;
  public string disabledHint;
  public bool once;
  public bool single;
  public string priority;
  public string pjsay;
  public string say;
  public List<DlgSeqItem> sequence;
  public bool detached;
  public List<List<string>> effect;
  public string jumpto;
  //transient
  public bool disabled;
}

public class DialogState
{
  public delegate void VVDelegate();
  public delegate void VIDelegate(int i);
  public int refCount = 0;
  public Dictionary<string, DlgNode> byId;
  public Dictionary<string, bool> once = new Dictionary<string, bool>();
  public Future onDone;
  public string npcName;

  public void Inc()
  {
    refCount++;
  }
  public void Dec()
  {
    refCount--;
  }
  public void Check()
  {
    if (refCount == 0)
      onDone.Notify();
  }
  public void PNJSays(string name, string text, Future.FutureDelegate cb)
  {
    // FILL ME
  }
  public void PJSays(List<string> choices, VIDelegate cb)
  {
    // FILL ME
  }
  public static bool Boolify(object what)
  {
    var rt = what.GetType();
    if (rt.Equals(typeof(int)))
      return (int)what != 0;
    else if (rt.Equals(typeof(long)))
      return (long)what != 0;
    else if (rt.Equals(typeof(uint)))
      return (uint)what != 0;
    else if (rt.Equals(typeof(ulong)))
      return (ulong)what != 0;
    else if (rt.Equals(typeof(string)))
      return (string)what != "";
    else if (rt.Equals(typeof(bool)))
      return (bool)what;
    else if (rt.Equals(typeof(double)))
      return (double)what != 0.0;
    else if (rt.Equals(typeof(float)))
      return (float)what != 0.0f;
    else
    {
      Debug.Log($"Value is not castable to bool: {what}");
      return false;
    }
  }
  public object CheckEval(string what)
  {
    try
    {
      var res = Eval.eval(what);
      return res;
    }
    catch (Exception e)
    {
      Debug.Log("Exception evaling '" + what + "' : " + e);
      return null;
    }
  }
  public void ProcessDetached(List<List<string>> effects, int index=0)
  {
    if (index >= effects.Count)
      return;
    var jfs = new List<Future>();
    foreach (var e in effects[index])
    {
      var o = CheckEval(e);
      var jf = o as Future;
      if (jf != null)
        jfs.Add(jf);
    }
    MultiWatch(jfs, delegate() { ProcessDetached(effects, index+1);});
  }
  public void MultiWatch(List<Future> jfs, VVDelegate cb, int index = 0)
  {
    Debug.Log($"MultiWatch {index} / {jfs.Count}");
    if (index >= jfs.Count)
    {
      cb();
      return;
    }
    jfs[index].Watch(delegate() { MultiWatch(jfs, cb, index+1);});
  }
  public string Expand(string what)
  {
    int start = 0;
    while (true)
    {
      var pos = what.IndexOf('$', start);
      if (pos == -1 || pos == what.Length - 1)
        break;
      string expr = "";
      if (what[pos+1] == '$')
      {
        what = what.Substring(0, pos) + what.Substring(pos+1);
        start = pos+1;
        continue;
      }
      if (what[pos+1] == '{')
      {
        var epos = what.IndexOf('}');
        if (epos == -1 || epos < pos)
        {
          Debug.Log("Unhandled/broken string for expansion: " + what);
          break;
        }
        expr = what.Substring(pos+2, epos-pos-2);
        what = what.Substring(0, pos) + what.Substring(epos+1);
      }
      else
      { // sep is space but we keep it
        var epos = what.IndexOf(' ', pos+1);
        if (epos == -1 || epos < pos)
        {
          Debug.Log("Unhandled/broken string for expansion: " + what);
          break;
        }
        expr = what.Substring(pos+1, epos-pos-1);
        what = what.Substring(0, pos-1) + what.Substring(epos);
      }
      var sval = "";
      try
      {
        var val = Eval.eval(expr);
        sval = $"{val}";
      }
      catch (Exception e)
      {
        Debug.Log("Exception evaling '" + expr + "' : " + e);
      }
      what = what.Substring(0, pos) + sval + what.Substring(pos);
      start = pos + sval.Length;
    }
    return what;
  }
  public List<DlgNode> Filter(List<DlgNode> input)
  {
    var maxPrio = 0.0;
    var res = new List<DlgNode>();
    foreach (var n in input)
    {
      if (n.condition != null && n.condition != "")
      {
        var condVal = Boolify(CheckEval(n.condition));
        if (!condVal)
        {
          Debug.Log("condition is false: " + n.condition);
          n.disabled = true;
          if (n.disabledHint == null)
            continue;
        } 
        else
        {
          Debug.Log("condition is true: " + n.condition);
          n.disabled = false;
        }
      } // n.condition
      if (n.once)
      {
        bool o;
        if (once.TryGetValue(n.id, out o))
          continue;
      }
      if (n.single)
        ; // FIXME implement
      if (n.priority != null && n.priority != "")
      {
        var np = Double.Parse(n.priority);
        if (np < maxPrio)
          continue;
        if (np > maxPrio)
          maxPrio = np;
      }
      if (n.pjsay != null && n.pjsay == "->")
      {
        var sublist = Filter(n.choices);
        foreach (var nn in sublist)
        res.Add(nn);
      }
      else
        res.Add(n);
    }
    if (maxPrio != 0.0)
    { // second pass
      var res2 = new List<DlgNode>();
      foreach(var n in res)
      {
        if (n.priority == null || n.priority == "" || Double.Parse(n.priority) < maxPrio)
          continue;
        res2.Add(n);
      }
      return res2;
    }
    return res;
  }
  public void ExecuteAfterSay(DlgNode n, bool didEffect, int effectPos)
  {
    Debug.Log("ExecuteAfterSay");
    if (!didEffect)
    {
      if (n.once)
        once[n.id] = true;
      if (n.single)
        ; // FIXME implement
      if (n.effect != null)
      {
        if (n.detached)
          ProcessDetached(n.effect);
        else
        {
          while (effectPos < n.effect.Count)
          {
            var jfs = new List<Future>();
            foreach(var e in n.effect[effectPos])
            {
              var res = CheckEval(e);
              var jf = res as Future;
              if (jf != null)
                jfs.Add(jf);
            }
            if (jfs.Count == 0)
            {
              ++effectPos;
              continue;
            }
            Inc();
            MultiWatch(jfs, delegate() {
                Dec();
                ExecuteAfterSay(n, false, effectPos + 1);
            });
            return;
          }
        }
      } // n.effect
    } // !didEffect
    if (n.jumpto != null && n.jumpto != "")
    {
      var sc = n.jumpto.IndexOf(':');
      if (sc != -1)
      {
        var file = n.jumpto.Substring(0, sc);
        var node = n.jumpto.Substring(sc+1);
        var filename = "dialogs/" + file + ".dlg";
        var res = Resources.Load<TextAsset>(filename);
        if (res == null)
          Debug.Log("### failed to load dialog file " + filename);
        var roots = JsonConvert.DeserializeObject<List<DlgNode>>(res.text);
        var tbyId = new Dictionary<string, DlgNode>();
        once.Clear();
        byId = tbyId;
        foreach(var nn in roots)
          Dialog.Index(nn, tbyId);
        if (node == "")
          FilterAndExec(roots);
        else
        {
          DlgNode start;
          if (!tbyId.TryGetValue(node, out start))
            Debug.Log("### Failed jumpto, node " + node + " does not exist");
          else
            Execute(start, false, false);
        }
      } // external jump
      else
      {
        DlgNode next;
        if (!byId.TryGetValue(n.jumpto, out next))
          Debug.Log("### Failed jumpto, id " + n.jumpto + " does not exist");
        Execute(next, false, false);
      } // internal jump
      return;
    } // jumpto
    ExecuteAfterJumpto(n);
    Check();
  }
  public void ExecuteAfterJumpto(DlgNode n)
  {
    if (n.choices != null && n.choices.Count != 0)
    {
      var choices = Filter(n.choices);
      Debug.Log($"choices: from {n.choices.Count} to {choices.Count}");
      if (choices.Count == 1 && (choices[0].pjsay == null || choices[0].pjsay == ""))
      {
        ExecuteAfterChoice(choices, 0, n, false, 0);
        Check();
        return;
      }
      var phrases = new List<string>();
      foreach(var c in choices)
      {
        var s = c.pjsay;
        if (s == null || s == "")
          s = "...";
        if (c.disabled)
          s = "#<color=#402020>" + s + c.disabledHint + "</color>";
        phrases.Add(s);
      }
      for (var i=0; i<phrases.Count; ++i)
        phrases[i] = Expand(phrases[i]);
      Inc();
      PJSays(phrases, delegate(int c) {
          Dec();
          ExecuteAfterChoice(choices, c, n, false, 0);
      });
    }
    if (n.list != null && n.list.Count != 0)
    {
      FilterAndExec(n.list);
    }
    Check();
  }
  public void ExecuteAfterChoice(List<DlgNode> choices, int choice, DlgNode n, bool didEffect, int effectIndex)
  {
    Debug.Log($"ExecuteAfterChoice chose {choice}");
    if (n.once)
        once[n.id] = true;
    if (n.single)
      ; // FIXME implement
    if (!didEffect)
    {
      if (n.effect != null)
      {
        if (n.detached)
          ProcessDetached(n.effect);
        else
        {
          while (effectIndex < n.effect.Count)
          {
            var jfs = new List<Future>();
            foreach(var e in n.effect[effectIndex])
            {
              var res = CheckEval(e);
              var jf = res as Future;
              if (jf != null)
                jfs.Add(jf);
            }
            if (jfs.Count == 0)
            {
              ++effectIndex;
              continue;
            }
            Inc();
            MultiWatch(jfs, delegate() {
                Dec();
                ExecuteAfterChoice(choices, choice, n, false, effectIndex + 1);
            });
            return;
          }
        }
      } // n.effect
    } // didEffect
    // Two possibilities, either the pnj response is in choices[choice],
    // or there are multiple responses in choices[choice].list
    Debug.Log("ExecuteAfterChoice going on");
    Inc();
    var selected = choices[choice];
    if (selected.list != null && selected.list.Count != 0)
      FilterAndExec(selected.list);
    else
      Execute(selected, true, false);
    if (n.list != null && n.list.Count != 0)
      FilterAndExec(n.list);
    Dec();
    Check();
  }
  public void ExecuteAfterPJSay(DlgNode n, int seqPos, bool pnjSpeaks, bool didEffect)
  {
    Debug.Log("ExecuteAfterPJSay");
    if (n.single)
      ; // FIXME implement
    if (n.once)
      once[n.id] = true;
    if (n.sequence != null)
    {
      for (var si = seqPos; si < n.sequence.Count; ++si)
      {
        var silocal = si;
        Future.FutureDelegate cb = delegate() { 
          Dec();
          ExecuteAfterPJSay(n, silocal+1, !pnjSpeaks, didEffect);
        };

        var seqitem = n.sequence[si];
        if (seqitem.text != null)
        {
          Inc();
          PNJSays(npcName, Expand(seqitem.text), cb);
          return;
        }
        if (seqitem.key == "effect")
        {
          var r = CheckEval(seqitem.value);
          var jf = r as Future;
          if (jf != null)
          {
            Inc();
            jf.Watch(cb);
            return;
          } // else go on
        }
        else if (seqitem.key == "speak")
        {
          var name = npcName;
          var text = seqitem.value;
          var p = text.IndexOf(')');
          var pp = text.IndexOf('(');
          if (p != -1 && (pp == -1 || pp > p))
          {
            name = text.Substring(0, p);
            text = text.Substring(p+1);
          }
          Inc();
          Future.FutureDelegate cb2 = delegate() {
            Dec();
            ExecuteAfterPJSay(n, silocal+1, false, didEffect);
          };
          var ex = Expand(text);
          if (name == "pj")
            PJSays(new List<string>{ex}, (i) => { cb2(); } );
          else
            PNJSays(name, ex, cb2);
          return;
        }
        else if (seqitem.key == "say")
        {
          Inc();
          PNJSays(npcName, Expand(seqitem.value), cb);
          return;
        }
        else
          Debug.Log("#### Not understood sequence key: " + seqitem.key);
      }
    } // n.sequence
    if (n.say != null && n.say != "")
    {
      Debug.Log("  eapjs say");
      var text = Expand(n.say);
      Inc();
      PNJSays(npcName, text, delegate() {
          Dec();
          ExecuteAfterSay(n, didEffect, 0);
      });
    }
    else
      ExecuteAfterSay(n, didEffect, 0);
    Check();
  }
  public void Execute(DlgNode n, bool pjspoke, bool didEffect)
  {
    Debug.Log("Execute");
    if (n.pjsay != null && n.pjsay != "" && !pjspoke)
    {
      Inc();
      PJSays(new List<string>() {Expand(n.pjsay)}, delegate(int i) { Dec(); ExecuteAfterPJSay(n, 0, true, didEffect);});
    }
    else
      ExecuteAfterPJSay(n, 0, true, didEffect);
  }
  public void FilterAndExec(List<DlgNode> choices)
  {
    var v = Filter(choices);
    Debug.Log($"FilterAndExec from {choices.Count} to {v.Count}");
    if (v.Count == 0)
      return;
    var choice = (int)(UnityEngine.Random.value * (double)(v.Count-0.000001));
    
    Execute(v[choice], false, false);
  }
}
public class Dialog
{
  static private int nextIndex = 1;
   
  static public void Index(DlgNode n, Dictionary<string, DlgNode> byId)
  {
    if (n.id == null || n.id == "")
    {
      n.id = "auto_" + (++nextIndex).ToString();
    }
    byId[n.id] = n;
    if (n.list != null)
      foreach(var nn in n.list)
         Index(nn, byId);
    if (n.choices != null)
      foreach(var nn in n.choices)
         Index(nn, byId);
  }
  static public Future StartDialog(string pnjName, string json)
  {
    var roots = JsonConvert.DeserializeObject<List<DlgNode>>(json);
    var byId = new Dictionary<string, DlgNode>();
    foreach(var n in roots)
      Index(n, byId);
    var onDone = new Future();
    var prevFace = "";
    var ds = new DialogState() {
      byId = byId,
      onDone = onDone,
      npcName = pnjName,
    };
    ds.FilterAndExec(roots);
    return onDone;
  }
}
