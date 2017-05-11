#include "sched.h"

/*
 * Se invoca cuando se deba escoger la siguiente tarea a ejecutar en la CPU
 */
static task_t* pick_next_task_fcfs(runqueue_t* rq) {
	task_t* t=head_slist(&rq->tasks); // Lista ordenada por longitud de ráfaga de CPU ( sólo debes elegir la primera)
    	
	//Seleccionamos la tarea y la borramos de la cola
	//porque ya no va a estar en cola.
    	if (t) {
		/* Current is not on the rq*/
		remove_slist(&rq->tasks,t);//borramos la tarea de la cola
		t->on_rq=FALSE;//la desactivamos
		rq->cur_task=t;//la seleccionamos como en uso
    	}
    
   	return t;
}

/*
 * Se invoca para encolar una tarea en la runqueue
 */
static void enqueue_task_fcfs(task_t* t, runqueue_t* rq, int runnable){

	if (t->on_rq || is_idle_task(t))//si la tarea está en cola, salimos.
        return;
	
	insert_slist(&rq->tasks,t); //insertamos la tarea en la cola
   	t->on_rq=TRUE;//la activamos, avisando que la tarea está en la cola

	
	//Si la tarea no se pudo correr antes en esta cola, se increamenta el contador de tareas pendientes de correr
    	if (!runnable){
		rq->nr_runnable++;
    	}
}

static void task_tick_fcfs(runqueue_t* rq){

    task_t* current=rq->cur_task;//Asignamos a current la tarea actual

    if (is_idle_task(current))//si la tarea actual es idle, salimos
        return;
    
    if (current->runnable_ticks_left==1) //Si le queda un tick decrementamos el contador de tareas a correr.
        rq->nr_runnable--; // The task is either exiting or going to sleep right now    
}

static task_t* steal_task_fcfs(runqueue_t* rq){
    task_t* t=tail_slist(&rq->tasks);
    
    if (t) {
        remove_slist(&rq->tasks,t);
        t->on_rq=FALSE;
        rq->nr_runnable--;
    }
    return t;    
}

sched_class_t fcfs_sched={
    .pick_next_task=pick_next_task_fcfs,
    .enqueue_task=enqueue_task_fcfs,
    .task_tick=task_tick_fcfs,
    .steal_task=steal_task_fcfs
};


