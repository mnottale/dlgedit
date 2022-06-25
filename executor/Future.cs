using System.Collections;

public class Future
{
  public delegate void FutureDelegate();
  ArrayList watchers = new ArrayList();
  public event FutureDelegate trigger;
  public bool triggered;
  public void Watch(FutureDelegate watch)
  {
    if (triggered)
    {
      watch();
    }
    watchers.Add(watch);
  }
  public void Notify()
  {
    foreach (FutureDelegate  a in watchers)
      a();
    if (trigger != null)
      trigger();
    triggered = true;
  }
};
