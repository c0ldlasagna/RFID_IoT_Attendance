import { redirect, type Actions } from "@sveltejs/kit";
import { fail } from "@sveltejs/kit";

export const actions = {
    login: async ({ request, cookies,locals:{supabase} }) => {
        const formData = await request.formData();
        const email = formData.get("email");
        const password = formData.get("password");
        
        if (!email || !password) {
            return fail(400,{fail:true});
        }

        const { data, error } =
            await supabase.auth.signInWithPassword({
                email: email.toString(),
                password: password.toString()
            });

        if (error) {
            return fail(400,{fail:true});
        }
        redirect(303,"/dashboard");
    }
} satisfies Actions;