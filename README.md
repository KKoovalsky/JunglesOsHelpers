# JunglesOsHelpers

This repo contains helpers for RTOSes. At this stage FreeRTOS is only supported. See below what helpers are currently
implemented.

## os_delayed_job

Implements `os_delayed_job_interface`. Is responsible for scheduling a job to be executed after specific period of
time. One can use it like that:

```
{
    jungles::os_delayed_job j;

    // Will be called after 1s from this call.
    j.start(1000, []() { debug_print("Some callback\r\n"); }); 

    // Detach the job so 'j' can be destructed before executing the callback.
    j.detach();
}

// The callback is still valid after detaching and will outlive the 'j' object.
```

When not detached, then `os_delayed_job` will block on destruction.

```
{
    jungles::os_delayed_job j;

    // Will be called after 1s from this call.
    j.start(1000, []() { debug_print("Some callback\r\n"); }); 

} // Will block here for 1s and will leave this scope after the callback is executed

```
