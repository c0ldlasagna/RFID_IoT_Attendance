<script lang="ts">
    import {type Session} from "$lib/session_status";
    import { enhance } from "$app/forms";
    interface Props{
        session : Session,
        register_name: string | null | undefined;
    }
    let {session = $bindable(),register_name=$bindable()}:Props = $props();

    const {session_id,session_status}  = $derived(session);
</script>

<form use:enhance method="POST" action="?/state" id="session-button-container" class="flex flex-col text-center gap-y-3" >
    <div id="session-status-container" 
    class="flex flex-col w-full rounded-md  items-start p-4 inset-ring" 
    class:inset-ring-green-500={session_status=="active"} 
    class:bg-green-900={session_status=="active"} 
    class:inset-ring-red-600={session_status=="idle"} 
    class:bg-red-900={session_status=="idle"} 
    class:inset-ring-orange-500={session_status=="late"} 
    class:bg-orange-900={session_status=="late"}
    class:bg-blue-950={session_status=="registering"}
    class:inset-ring-blue-600={session_status=="registering"}>
            <h1 class="font-display text-white justify-self-start text-xl">
                Current Session
            </h1>
            <div class="flex flex-row">
                <h2 class="font-display text-white text-4xl mt-3">
                    {session_id}:{session_status.toUpperCase()}{session_status=="registering" ? ": " + register_name : null}
                </h2>
            </div>
    </div>
    <div class="flex flex-row gap-2">
        <button name="session_status" class="session-button ring-1 ring-green-500 hover:bg-green-900  " value="active" type="submit">
            Active
        </button>
        <button name="session_status" class="session-button ring-1 grow ring-orange-500 hover:bg-orange-900  " value="late" type="submit">
            Late
        </button>
        <button name="session_status" class="session-button ring-1 grow ring-red-600 hover:bg-red-900" value="idle" type="submit">
            Idle
        </button>
    </div>
    <div class="flex flex-col bg-blue-950 rounded-md items-start p-4 ring ring-blue-600">
        <h1 class="font-display text-white text-xl">
                Register a new Card
            </h1>
            <p class="text-justify font-display text-sm text-white">
                To register a new UID, type a name for the new UID, and hit register. Wait for the status light to turn blue then scan your new card.
            </p>
            <input
                class="font-display w-full text-blue-300 focus:outline-none focusfocus:outline-none border-b border-b-white"
                placeholder="Name"
                name="register_name"
                type="text"
                onkeydown={(e) => {
                    if (e.key === 'Enter') {
                        e.preventDefault();
                        document
                            .getElementById('register-button')
                            ?.click();
                    }
                }}
            />        </div>
        <button id="register-button" name="session_status" class="session-button ring ring-blue-600 hover:bg-blue-900" value="registering" type="submit">
            Register
        </button>
</form>


<style>
    .session-button{
        cursor: pointer;
        padding: 4px;
        border-radius: var(--radius-md);
        color: white;
        flex:1;
        font-family: "JetBrains Mono";
        max-height: fit-content;
    }
</style>