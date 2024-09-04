import { useRef } from 'react'
import emailjs from '@emailjs/browser'

const FeedbackForm = () => {
	const form = useRef()

	const sendEmail = (e) => {
		e.preventDefault()

		emailjs
			.sendForm(
				'service_rstijfs',
				'template_xz3gfcy',
				form.current,
				'9pau7xYq6QNcQn13S'
			)
			.then(
				(result) => {
					console.log(result.text)
				},
				(error) => {
					console.log(error.text)
				}
			)
	}

	return (
		<div className="mt-10 px-20 h-80 bg-secondary flex justify-center items-center">
			<div className="ml-32">
				<form
					className="grid grid-cols-2 gap-x-4"
					ref={form}
					onSubmit={sendEmail}
				>
					<div className="space-y-3">
						<label>
							<input
								type="text"
								name="user_name"
								placeholder=" Full Name"
								className="py-2 mt-2 w-full rounded-lg px-18"
							/>
						</label>

						<label>
							<input
								type="email"
								name="user_email"
								placeholder=" Email"
								className="py-2 mt-2 w-full rounded-lg px-18"
							/>
						</label>

						<label>
							<input
								name="message"
								placeholder=" Message"
								className="h-20 mt-2 w-full py-2 rounded-lg px-18"
							/>
						</label>
					</div>

					<div className="flex flex-col justify-end items-center text-white text-2xl">
						<p className="text-center">
							We appreciate your opinion, so send us some
							Feedback, or maybe a joke 🤗
						</p>
						<button
							className="px-10 py-2 mt-10 text-lg rounded-lg bg-primary hover:bg-orange-500"
							type="submit"
							disabled
						>
							Submit
						</button>
					</div>
				</form>
			</div>
		</div>
	)
}

export default FeedbackForm
