<script lang='ts'>
    import { enhance } from '$app/forms';

    let password:string = $state("");
    let username:string = $state("");

    let {form} = $props();

</script>

<div class = "bg-bg font-bold font-display bg-cover w-screen h-screen flex items-center justify-center bg-center">
    <form use:enhance method="post" action="?/login" class = "items-start gap-8 justify-center content-evenly p-20 text-white flex flex-col ring-2 ring-accent w-1/2 h-1/2 rounded-xl">
        <h1 class="text-5xl align-top">Log In</h1>
        <input name="email" bind:value={username} type="email" placeholder="Email" class = "max-w-6/7 m-2 focus:outline-none focus:border-b focus:border-b-white pb-1"/>
        <input name="password" bind:value={password} type="password" placeholder="Password" class = "max-w-6/7 m-2 focus:outline-none focus:border-b focus:border-b-white pb-1"/>
        <button type="submit" class = "cursor-pointer max-w-1/4 ring-1 ring-accent rounded-sm p-2 hover:inset-ring-1 hover:inset-ring-accent">
            Login
        </button>
        {#if form?.fail}
            <p class="font-display text-red-600">
                Login Failed
            </p>
        {/if}
    </form>
</div>
