<script lang="ts">
    import type { SessionState } from "$lib/session_status.js";
    import SessionControls from "$lib/components/SessionControls.svelte";
    import { onMount } from "svelte";
    import { invalidateAll } from "$app/navigation";
    import Table from "$lib/components/Table.svelte";
    let {data} = $props()

    onMount(()=>{
        const interval = setInterval(()=>{
            invalidateAll()
        }, 2000);

        return ()=>clearInterval(interval);
    });

    const attendanceColumns = [
    { key: "session",label:"Session"},
    { key: "name", label: "Name" },
    { key: "uid", label: "UID" },
    { key: "status", label: "Status" },
    { key: "timestamp", label: "Time" }
    ];

    const userColumns = [
        {key: "name",label:"Name"},
        {key: "uid", label:"UID"}
    ]

    const sessionColumns = [
        {key:"id",label:"ID"},
        {key:"start_time",label:"Start Time"},
        {key:"end_time",label:"End Time"}
    ]

</script>

<div class="flex flex-col bg-bg p-5 h-screen">
    <div class="flex-row flex gap-x-5 h-full">
        <div class="w-1/2 flex flex-col gap-y-3">
            <SessionControls bind:session={data.current_session} bind:register_name={data.register_name}/>
            <div class="flex-1 min-h-1">
                <Table
                columns={userColumns}
                rows={data.registered_cards}
                />    
            </div>
            <form action="?/logout" method="post">
                <button type="submit"  class="ring ring-red-600 w-full text-white font-display rounded-md h-10 hover:bg-red-900">
                    Logout
                </button>
            </form>
        </div>
        <div class="w-1/2 flex flex-col h-full gap-y-3">
            <div class="flex-1 min-h-0">
                <Table
                    columns={sessionColumns}
                    rows={data.sessions}/>
            </div> 
            <div class="flex-1 min-h-0">
                <Table
                columns={attendanceColumns} 
                rows={data.scan_logs}
                />
            </div>
        </div>
    </div>
</div>
