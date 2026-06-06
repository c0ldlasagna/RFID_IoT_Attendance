import { error, type RequestHandler } from "@sveltejs/kit"
import { get_current_session, type Session, type SessionState } from "$lib/session_status";
import type { SupabaseClient } from "@supabase/supabase-js";
import { register } from "module";

interface scan_request{
    uid:string;
};

interface registered_card{
    uid:string;
    name:string;
}

const uidExists = async (supabase:SupabaseClient,uid:string):Promise<registered_card|null> =>{
    // Check if UID is registered
    console.log(`Checking for uid ${uid}`);
    const {data,error} = await supabase.from("registered_cards").select().eq("uid",uid);
    console.log(data,error);
    if(error || !data.length){
        return null;
    }
    return data[0];
};  

const checkDuplicate = async(supabase:SupabaseClient,uid:string,session : Session):Promise<Boolean> =>{
    const {data} = await supabase.from("scan_logs").select().eq("uid",uid).eq("session",session.session_id);
    console.log(data);
    if(data?.length){
        return true;
    }
    return false;
};

const setIdleSession = async(supabase:SupabaseClient,session:Session)=>{
    await supabase.from("sessions").update({"end_time": new Date().toISOString()}).eq("id",session.session_id);
    await supabase.from("sessions").insert({"status":"idle"});
}

const handleRegistration = async(supabase:SupabaseClient,uid:string,session:Session) =>{
    const {error} = await supabase.from("registered_cards").insert({"uid":uid,"name":session.register_name});
    console.log(error);
    setIdleSession(supabase,session);
    if(error){
        return Response.json(
            {
                "message":"failed to register card"
            },
            {
                status:500
            }
        );
    }
    return Response.json(
        {
            "message":"Successfully registered card"
        },
        {
            status:201
        }
    )
}

export const POST: RequestHandler = async ({request,locals:{supabase}}) =>{
    
    // Convert request to JSON
    const req:scan_request  = await request.json();
    const current_session:Session = await get_current_session(supabase);
    console.log(current_session);

    const scanned_card = await uidExists(supabase,req.uid);
    
    // Check if UID is in the database
    if(!scanned_card){
        if(current_session.session_status=="registering"){
           return handleRegistration(supabase,req.uid,current_session);
        }
        return Response.json(
            {
                "message":"UID not found."
            },
            {
                status:404
            }
        )
    }

    if(current_session.session_status=="registering"){
        return Response.json(
            {
                "message":"UID already scanned registered"
            },
            {
                status:404
            }
        )
    }

    // Check if card already scanned during current session
    if(await checkDuplicate(supabase,scanned_card.uid,current_session)){
        return Response.json(
            {
                "message":"UID already scanned this session"
            },
            {
                status:409
            }
        )
    }

    if(current_session.session_status=="idle"){
        return Response.json(
        {
            "message":"No session",
            "uid":scanned_card.uid,
            "session":current_session,
            "name":scanned_card.name

        },
        {
            status: 404
        }
    )
    };

    // Add scan log to database
    const {data,error} = await supabase.from("scan_logs").insert({
        "uid":scanned_card.uid,
        "session":current_session.session_id,
        "name":scanned_card.name,
        "status":current_session.session_status
    });

    if(error){
        return Response.json(
        {
            "message": "Failed to record attendance data",
            "uid":scanned_card.uid,
            "session":current_session,
            "name":scanned_card.name
        },
        {
            status: 400
        }
    );

    }
    return Response.json(
        {
            "message":"Successfully recorded attendance data",
            "uid":scanned_card.uid,
            "session":current_session,
            "name":scanned_card.name
        },
        {
            status: current_session.session_status=="late" ? 408 : 200 
        }
    )
}