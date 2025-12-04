import { ThreeDot, OrbitProgress } from "react-loading-indicators";
import { useEffect, useState, useCallback } from "react";

function FactGenerator() {

    const [fact, setFact] = useState('');
    const [loading, setLoading] = useState(false);
    const [errorMsg, setErrorMsg] = useState(''); // NEW: track errors

    // NEW — helper function: simulate a tiny delay (fake network delay)
    const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

    // renamed function + wrapped with useCallback to avoid re-creation
    const fetchRandomFact = useCallback(async () => {

        if (loading) {
            console.log("Request ignored: previous request still running");
            return;
        }

        try {
            setLoading(true);
            setErrorMsg('');

            // NEW: simulate latency to make UI smoother
            await sleep(300);

            const response = await fetch("https://uselessfacts.jsph.pl/api/v2/facts/random?language=en");
            if (!response.ok) {
                throw new Error("Failed to fetch fact");
            }

            const json = await response.json();
            console.log("Fetched fact:", json);

            setFact(json.text);
        } 
        catch (err) {
            console.log("Error:", err);
            setErrorMsg("Could not load fact. Please try again.");
        }
        finally {
            setLoading(false);
        }

    }, [loading]);

    // NEW — log when fact changes
    useEffect(() => {
        if (fact) console.log("Updated fact:", fact);
    }, [fact]);

    // component mount effect
    useEffect(() => {
        console.log("Component mounted: fetching initial fact");
        fetchRandomFact();
    }, [fetchRandomFact]);

    return (
        <div>
            <h1 className="text-white font-extrabold text-3xl mb-10">
                Fact Generator
            </h1>

            <div className="m-10">

                {loading ? (
                    <OrbitProgress
                        variant="spokes"
                        dense
                        color="white"
                        size="medium"
                    />
                ) : errorMsg ? (
                    <p className="text-red-400 font-bold text-xl">{errorMsg}</p>
                ) : (
                    <p className="text-2xl text-white font-semibold">
                        {fact}<br />
                    </p>
                )}

            </div>

            <button
                className="font-bold bg-gray-900 p-3 rounded-xl cursor-pointer
                hover:outline-white hover:outline-2
                hover:scale-105 hover:bg-gray-800
                duration-75 ease-in"
                onClick={fetchRandomFact}
            >
                Get New Fact
            </button>

        </div>
    );
}

export default FactGenerator;
